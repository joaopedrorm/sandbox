#include <stdio.h>

enum weapon {
	none = 0,
	sword = 1,
	spear = 2,
	dagger = 4,
	axe = 8
};

void print_weapon(enum weapon w) {
	if ((sword & w) == sword) {
		printf(" sword ");
	}
	if ((spear & w) == spear) {
		printf(" spear ");
	}
	if ((dagger & w) == dagger) {
		printf(" dagger ");
	}
	if ((axe & w) == axe) {
		printf(" axe ");
	}
	if (w == none) {
		printf(" none ");
	}
}

int main(int argc, char * argv[]) {
	printf("Weapon=");
	print_weapon(sword | axe);
	return 0;
}
