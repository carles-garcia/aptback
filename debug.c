#include "debug.h"

void debug_args(struct arguments args) {
  printf("dat: %d-%d-%d-%d-%d-%d\n", args.dat.year, args.dat.month, args.dat.day, args.dat.hour,args.dat.minute,args.dat.second);
  printf("until: %d-%d-%d-%d-%d-%d\n", args.until.year, args.until.month, args.until.day,args.until.hour,args.until.minute,args.until.second);
  if (args.option == INSTALL) printf("install\n");
  if (args.option == REMOVE) printf("remove\n");
  if (args.option == UPGRADE) printf("upgrade\n");
  if (args.removed) printf("-o removed\n");
  if (args.installed) printf("-o installed\n");
  if (args.upgraded) printf("-o upgraded\n");
}

void debug_actions(struct action **actions, int num_act) {
  int i;
  for (i = 0; i < num_act; ++i) {
    //int j;
    //for (j = 0; j < actions[i]->num_pack; ++j) {
      printf("%d",actions[i]->date.year);
      printf("\n");
    //}
  }
}

