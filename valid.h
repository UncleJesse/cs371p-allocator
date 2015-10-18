
// iterate through Allocator's array and ensure that only valid blocks
// have been allocated and the amount of leftover space is also valid
bool valid() const{
	int i = 0;
	while (i < N){
		int s = (*this)[i];
		if (s == 0) // invalid sentinal value
			return false;
		else if (s < 0) {  //skips past allocated block
			int r = (*this)[i + s + sizeof(int)];
			if (s != r)
				return false; // make sure that both sentinals match
            i += -s + (2 * sizeof(int)); // increment i past second sentinal, to the sentinal of the next block
        }
        //checks unallocated block to make sure that there is enough 
        //space for two sentinels and at least 1 T
        else if (s > 0) {  
            if (s < sizeof(T) + 2 * sizeof(int)) // if block is too small, return false, block is invalid
            	return false;
            else{
            	int q = (*this)[i + s + sizeof(int)];
            	if(s != q)
            		return false; // both sentinals for this block should match
            	i += s + (2 * sizeof(int)); // increment i to next block
            }
        }
        if (i > N)
        	return false;
	}
	return true;
}