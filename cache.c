#include <stdio.h>
#include <math.h>



// Omitting offset; don't need it for this simulation
int isHitOrMissForDirectMapped( int tag, int index );
// int isHitOrMissFor
int cache_total_size;
int bytes_per_block;
int n_way;
int byte_addresses[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 0, 4, 8, 12, 16, 71, 3, 41, 81, 39, 38, 71, 15, 39, 11, 51, 57, 41 };
int size = (sizeof(byte_addresses) / sizeof(byte_addresses[0]));
struct row {int valid; int tag; int timestamp;};
struct row cache[5000];
int time = 0;

int findBits(int slots) {
	return (int) (log(slots) / log(2));	
}


int extract(int value, int begin, int end)
{
	int mask = (1 << (end - begin)) - 1;
	return (value >> begin) & mask;
}

int isHitOrMissForDirectMapped( int tag, int index ) {
	int rowIdx = 0;
	int isHit;
	// For direct-mapped, index is the cache line number.
	if ( cache[index].valid && cache[index].tag == tag ) {
		isHit = 1;
	}	else {
		isHit = 0;
		struct row line;
		line.valid = 1;
		line.tag = tag;
		cache[index] = line;
	}
	return isHit;
}

int getLowestTimeStamp(int n, int index) {
	int low = 5000;
	int lowIdx = -1;
	int rowIdx;
	for (rowIdx = n*index; rowIdx < n*index + n; rowIdx++) {
		if (cache[rowIdx].timestamp < low) {
			low = cache[rowIdx].timestamp;
			lowIdx = rowIdx;			
		}	
	}
	return lowIdx;
}

int isHitOrMissForSetAssoc( int n, int tag, int index ) {
	int isHit = 0;  // Initialize isHit to default value:  false
	int rowIdx = 0;
	// For set associative, index is the set number.
	for ( rowIdx = n*index; rowIdx < n*index + n; rowIdx++) {
		if ( cache[rowIdx].valid && cache[rowIdx].tag == tag ) {
			isHit = 1;
			break;
		}
	}
	// Now, isHit has value true if and only if we found a hit.
	if (isHit) return 1; // return true
	// Else search for cache line with valid field == 0 (false)
	for ( rowIdx = n*index; rowIdx < n*index + n; rowIdx++ )
		if ( cache[rowIdx].valid  == 0 ) {
			struct row line;
			line.valid = 1;
			line.tag = tag;
			line.timestamp = time;
			time++;
			cache[rowIdx] = line;
			break;
		}
	// If we didn't find a cache line with valid field false, then evict cache line
	if (rowIdx >= n*index + n) { // if falied to find invalid cache line
		int lowestTimeStamp;
		lowestTimeStamp = getLowestTimeStamp(n, index);
			cache[lowestTimeStamp].valid = 1;
			cache[lowestTimeStamp].tag = tag;
			cache[lowestTimeStamp].timestamp = time;
			time++;
		}         
	return isHit;
}


int answer;


int main () {
	char ans[100];
	printf("Cache total size (in bytes)?: ");
	scanf("%d", &cache_total_size);
	printf("Bytes per block?: ");
	scanf("%d", &bytes_per_block);
	printf("Direct-mapped cache? [y/N]: ");
	scanf("%s", &ans);

	if (ans[0] == 'y') {
		int num_index_bits = findBits(cache_total_size / bytes_per_block);
		int num_offset_bits = findBits(bytes_per_block);
		int num_tag_bits = (32 - num_index_bits) - num_offset_bits;
		

		for (int ii = 0; ii < size; ii++) {
			int addr = *(byte_addresses + ii);	
			int tag = extract(addr, num_offset_bits + num_index_bits, 32);
			int index = extract(addr, num_offset_bits, num_offset_bits + num_index_bits);
			int offset = extract(addr, 0, num_offset_bits);
			
			answer = isHitOrMissForDirectMapped( tag, index );
			if (answer)
				printf("%4d: HIT  (Tag/Set#/Offset: %d/%d/%d)\n", addr, tag, index, offset);
			else
				printf("%4d: MISS (Tag/Set#/Offset: %d/%d/%d)\n", addr, tag, index, offset);
		}
		} else {
		printf("Fully associative cache [y/N]: ");
		scanf("%s", &ans);
		if (ans[0] == 'y') {
			n_way = cache_total_size / bytes_per_block;
		} else {
			printf("Then it must be n-way set associative; What is n?: ");
			scanf("%d", &n_way);
		}
		int num_index_bits = findBits((cache_total_size / bytes_per_block) / n_way);
		int num_offset_bits = findBits(bytes_per_block);
		int num_tag_bits = (32 - num_index_bits) - num_offset_bits;
		

		for (int ii = 0; ii < size; ii++) {
			int addr = *(byte_addresses + ii);
			int tag = extract(addr, num_offset_bits + num_index_bits, 32);
			int index = extract(addr, num_offset_bits, num_offset_bits + num_index_bits);
			int offset = extract(addr, 0, num_offset_bits);
			answer = isHitOrMissForSetAssoc(n_way, tag, index);	
			if (answer) 
				printf("%4d: HIT  (Tag/Set#/Offset: %d/%d/%d)\n", addr, tag, index, offset);
			else
				printf("%4d: MISS (Tag/Set#/Offset: %d/%d/%d)\n", addr, tag, index, offset);
		}
	}
	return 0;  // The fnc main returns 0 for success.  Anything else is error number.
}
