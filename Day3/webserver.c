// omp_ws_taskmgr.c
// build: gcc -O2 -fopenmp omp_ws_taskmgr.c -o ws_taskmgr
// run:   OMP_NUM_THREADS=4 OMP_MAX_ACTIVE_LEVELS=2 ./ws_taskmgr
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_CONN 128

static int conn_id[MAX_CONN];
static int conn_alive[MAX_CONN];   // 1 = alive, 0 = please stop
static int conn_in_use[MAX_CONN];  // slot allocated
static int next_id = 1;

static void busy(int cycles) {
    volatile int s = 0;
    for (int i = 0; i < cycles; ++i) s += i;
}

static void handle_connection(int idx, int id) {
    printf("[conn %02d] start (slot %d, outer tid %d)\n", id, idx, omp_get_thread_num());

    // nested team: 2 threads: RX / TX
    #pragma omp parallel num_threads(2)
    {
        int lid = omp_get_thread_num();
        while (1) {
            int alive;
            #pragma omp atomic read
            alive = conn_alive[idx];
            if (!alive) break;

            if (lid == 0) {  // RX loop
                busy(900000);
                printf("[conn %02d][RX] tick (lvl=%d, tid=%d)\n", id, omp_get_level(), omp_get_thread_num());
            } else {         // TX loop
                busy(700000);
                printf("[conn %02d][TX] tick (lvl=%d, tid=%d)\n", id, omp_get_level(), omp_get_thread_num());
            }
        }

        #pragma omp barrier
        if (lid == 0) printf("[conn %02d] closing\n", id);
    }

    // mark slot free
    #pragma omp atomic write
    conn_in_use[idx] = 0;
}

static int find_free_slot(void) {
    for (int i = 0; i < MAX_CONN; ++i) {
        int used;
        #pragma omp atomic read
        used = conn_in_use[i];
        if (!used) return i;
    }
    return -1;
}

static int find_idx_by_id(int id) {
    for (int i = 0; i < MAX_CONN; ++i) {
        int used, cid;
        #pragma omp atomic read
        used = conn_in_use[i];
        if (!used) continue;
        #pragma omp atomic read
        cid = conn_id[i];
        if (cid == id) return i;
    }
    return -1;
}

static void help(void) {
    puts("commands: add [n] | kill <id> | list | quit");
}

int main(void) {
    // allow nested teams
    omp_set_dynamic(0);
    omp_set_max_active_levels(2);

    memset(conn_id, 0, sizeof conn_id);
    memset(conn_alive, 0, sizeof conn_alive);
    memset(conn_in_use, 0, sizeof conn_in_use);

    printf("server: pool=%d threads, nested max levels=%d\n",
           omp_get_max_threads(), omp_get_max_active_levels());
    help();

    #pragma omp parallel
    {
        #pragma omp single
        {
            char line[128];
            while (1) {
                printf("cmd> "); fflush(stdout);
                if (!fgets(line, sizeof line, stdin)) break;

                // trim newline
                line[strcspn(line, "\r\n")] = 0;

                if (strncmp(line, "add", 3) == 0 || strncmp(line, "a", 1) == 0) {
                    int n = 1;
                    sscanf(line, "%*s %d", &n);
                    if (n < 1) n = 1;

                    for (int k = 0; k < n; ++k) {
                        int slot = find_free_slot();
                        if (slot < 0) {
                            puts("no free slots");
                            break;
                        }
                        int id = next_id++;

                        #pragma omp atomic write
                        conn_in_use[slot] = 1;
                        #pragma omp atomic write
                        conn_id[slot] = id;
                        #pragma omp atomic write
                        conn_alive[slot] = 1;

                        printf("server: accepted conn %02d (slot %d)\n", id, slot);

                        // spawn long-lived task
                        #pragma omp task firstprivate(slot, id) shared(conn_alive, conn_in_use, conn_id)
                        {
                            // faux handshake
                            busy(400000);
                            printf("[conn %02d] handshake OK\n", id);
                            handle_connection(slot, id);
                        }
                    }
                } else if (strncmp(line, "kill", 4) == 0 || strncmp(line, "k ", 2) == 0) {
                    int id;
                    if (sscanf(line, "%*s %d", &id) == 1) {
                        int idx = find_idx_by_id(id);
                        if (idx >= 0) {
                            int alive;
                            #pragma omp atomic read
                            alive = conn_alive[idx];
                            if (alive) {
                                #pragma omp atomic write
                                conn_alive[idx] = 0;
                                printf("server: kill signaled for conn %02d (slot %d)\n", id, idx);
                            } else {
                                printf("server: conn %02d already stopping\n", id);
                            }
                        } else {
                            printf("server: conn %02d not found\n", id);
                        }
                    } else {
                        puts("usage: kill <id>");
                    }
                } else if (strncmp(line, "list", 4) == 0 || strncmp(line, "l", 1) == 0) {
                    puts("active connections:");
                    for (int i = 0; i < MAX_CONN; ++i) {
                        int used;
                        #pragma omp atomic read
                        used = conn_in_use[i];
                        if (!used) continue;
                        int id, alive;
                        #pragma omp atomic read
                        id = conn_id[i];
                        #pragma omp atomic read
                        alive = conn_alive[i];
                        printf("  slot %3d -> id %02d  [%s]\n", i, id, alive ? "alive" : "stopping");
                    }
                } else if (strncmp(line, "quit", 4) == 0 || strncmp(line, "q", 1) == 0 || strncmp(line,"exit",4)==0) {
                    // broadcast stop
                    for (int i = 0; i < MAX_CONN; ++i) {
                        int used;
                        #pragma omp atomic read
                        used = conn_in_use[i];
                        if (!used) continue;
                        #pragma omp atomic write
                        conn_alive[i] = 0;
                    }
                    puts("server: shutting down...");
                    break;
                } else if (strncmp(line, "help", 4) == 0 || strncmp(line, "h", 1) == 0 || line[0] == '?') {
                    help();
                } else if (line[0] == 0) {
                    continue;
                } else {
                    puts("unknown command; try: add [n] | kill <id> | list | quit");
                }
            }

            // wait for all live connection tasks to finish
            #pragma omp taskwait
        }
    }

    puts("server: all connections done; bye.");
    return 0;
}
