#include "src/hoffman_zip.h"
int main() {
	hfm::compress("1.txt");
	hfm::extract("1.huf","2.txt");
	return 0;
}