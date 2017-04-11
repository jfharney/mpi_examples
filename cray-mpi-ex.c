#define _GNU_SOURCE 
#include <stdio.h>
#include <sched.h>
#include <string.h>
#include "mpi.h"
/* Borrowed from util-linux-2.13-pre7/schedutils/taskset.c */
static char *cpuset_to_cstr(cpu_set_t *mask, char *str)
{
char *ptr = str;
int i, j, entry_made = 0;
for (i = 0; i < CPU_SETSIZE; i++) {
if (CPU_ISSET(i, mask)) {
int run = 0;
entry_made = 1;
for (j = i + 1; j < CPU_SETSIZE; j++) {
if (CPU_ISSET(j, mask)) run++;
else break;
 }
if (!run)
sprintf(ptr, "%d,", i);
else if (run == 1) {
sprintf(ptr, "%d,%d,", i, i + 1);
i++;
} else {
sprintf(ptr, "%d-%d,", i, i + run);
i += run;
}
while (*ptr != 0) ptr++;
}
}
ptr -= entry_made;
*ptr = 0;
return(str);
}
int main(int argc, char *argv[])
{
int rank, thread, i;
cpu_set_t coremask;
char clbuf[7 * CPU_SETSIZE], hnbuf[64], node[64];
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
memset(clbuf, 0, sizeof(clbuf));
memset(hnbuf, 0, sizeof(hnbuf));
(void)gethostname(hnbuf, sizeof(hnbuf));

        /*Remove nid from node name*/
  for (i=3; hnbuf[i] != '\0'; i++)
  {
    node[i-3] = hnbuf[i];
  }

{
(void)sched_getaffinity(0, sizeof(coremask), &coremask);
cpuset_to_cstr(&coremask, clbuf);
printf("Rank %d, Node %s, Core %s\n", rank, node, clbuf);
}
MPI_Finalize();
return(0);
}
