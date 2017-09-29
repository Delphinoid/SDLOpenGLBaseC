float fastInvSqrt(float x){
	/* Black magic perfected by some very clever people. */
	const float halfX = x*0.5f;
	const int *ir = (int *)&x;  // Dereferenced on next line to avoid warnings
	const int i = 0x5f3759df - (*ir>>1);
	const float *xr = (float *)&i;  // Dereferenced on next line to avoid warnings
	x = *xr;
	x *= 1.5f-halfX*x*x;  // Initial Newton-Raphson iteration (repeat this line for more accurate results)
	//x *= 1.5f-halfX*x*x;
	return x;
}
