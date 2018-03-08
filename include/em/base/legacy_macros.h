//
// Created by josem on 3/8/18.
//

#ifndef EM_CORE_LEGACY_MACROS_H
#define EM_CORE_LEGACY_MACROS_H

/** For all direct elements in the complex array in FFTW format.
 *
 * This macro is used to generate loops for the volume in an easy way. It
 * defines internal indexes 'k','i' and 'j' which ranges the volume using its
 * physical definition. It also defines 'kp', 'ip' and 'jp', which are the logical coordinates
 * It also works for 1D or 2D FFTW transforms
 *
 * @code
 * FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(V)
 * {
 *     int r2 = jp*jp + ip*ip + kp*kp;
 *
 *     std::cout << "element at physical coords: "<< i<<" "<<j<<" "<<k<<" has value: "<<DIRECT_A3D_ELEM(m, k, i, j) << std::endl;
 *     std::cout << "its logical coords are: "<< ip<<" "<<jp<<" "<<kp<<std::endl;
 *     std::cout << "its distance from the origin = "<<sqrt(r2)<<std::endl;
 *
 * }
 * @endcode
 */
#define FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(V, adim) \
    for (long int k = 0, kp = 0; k<adim.z; k++, kp = (k < adim.x) ? k : k - adim.z) \
    	for (long int i = 0, ip = 0 ; i<adim.y; i++, ip = (i < adim.x) ? i : i - adim.y) \
    		for (long int j = 0, jp = 0; j<adim.x; j++, jp = j)

/** For all direct elements in the complex array in FFTW format.
 *  The same as above, but now only for 2D images (this saves some time as k is not sampled
 */
#define FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM2D(V, adim) \
	for (long int i = 0, ip = 0 ; i<adim.y; i++, ip = (i < adim.x) ? i : i - adim.y) \
		for (long int j = 0, jp = 0; j<adim.x; j++, jp = j)

#endif //EM_CORE_LEGACY_MACROS_H
