//
// Created by josem on 3/8/18.
//

#ifndef EM_CORE_LEGACY_MACROS_H
#define EM_CORE_LEGACY_MACROS_H

#include "em/image/image.h"

//======================= From macros.h ====================================
/** Starting point for Xmipp volume/image
 *
 * Given a size (in some direction), this function returns the first index for
 * a volume/image/array with this size. The formula is -(int) ((float) (size)
 * / 2.0)
 */
#define FIRST_XMIPP_INDEX(size) -(long int)((float) (size) / 2.0)


// ====================== From multidim_array.h ============================
/** Returns the first X valid logical index
 */
#define STARTINGX(v) ((v).xinit)

/** Returns the last X valid logical index
 */
#define FINISHINGX(v) ((v).xinit + (v).xdim - 1)

/** Returns the first Y valid logical index
 */
#define STARTINGY(v) ((v).yinit)

/** Returns the last Y valid logical index
 */
#define FINISHINGY(v) ((v).yinit + (v).ydim - 1)

/** Returns the first Z valid logical index
 */
#define STARTINGZ(v) ((v).zinit)

/** Returns the last Z valid logical index
 */
#define FINISHINGZ(v) ((v).zinit + (v).zdim - 1)

/** Access to X dimension (size)
 */
#define XSIZE(v) ((v).xdim)

/** Access to Y dimension (size)
 */
#define YSIZE(v) ((v).ydim)

/** Access to Z dimension (size)
 */
#define ZSIZE(v) ((v).zdim)

/** Access to N dimension (size)
 */
#define NSIZE(v) ((v).ndim)

/** Access to XY dimension (Ysize*Xsize)
 */
#define YXSIZE(v) ((v).yxdim)

/** Access to XYZ dimension (Zsize*Ysize*Xsize)
 */
#define ZYXSIZE(v) ((v).zyxdim)

/** Access to XYZN dimension (Nsize*Zsize*Ysize*Xsize)
 */
#define MULTIDIM_SIZE(v) ((v).nzyxdim)

/** Access to XYZN dimension (Nsize*Zsize*Ysize*Xsize)
 */
#define NZYXSIZE(v) ((v).nzyxdim)

/** Array access.
 *
 * This macro gives you access to the array (T **)
 */
#ifndef MULTIDIM_ARRAY
#define MULTIDIM_ARRAY(v) ((v).data)
#endif

/** Access to a direct element.
 * v is the array, l is the image, k is the slice, i is the Y index and j is the X index.
 * i and j) within the slice.
 */
#define DIRECT_NZYX_ELEM(v, l, k, i, j) ((v).data[(l)*ZYXSIZE(v)+(k)*YXSIZE(v)+((i)*XSIZE(v))+(j)])

/** Multidim element: Logical access.
 */
#define NZYX_ELEM(v, l, k, i, j)  \
    DIRECT_NZYX_ELEM((v), (l), (k) - STARTINGZ(v), (i) - STARTINGY(v), (j) - STARTINGX(v))

/** Access to a direct element.
 * v is the array, k is the slice and n is the number of the pixel (combined i and j)
 * within the slice.
 */
#define DIRECT_MULTIDIM_ELEM(v,n) ((v).data[(n)])

/** For all direct elements in the array
 *
 * This macro is used to generate loops for the array in an easy manner. It
 * defines an internal index 'n' which goes over the slices and 'n' that
 * goes over the pixels in each slice.
 *
 * @code
 * FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(v)
 * {
 *     std::cout << DIRECT_MULTIDIM_ELEM(v,n) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(v) \
    for (long int n=0; n<NZYXSIZE(v); ++n)

/** For all direct elements in the array
 *
 * This macro is used to generate loops for the array in an easy
 * manner. It defines internal indexes 'l', 'k','i' and 'j' which
 * ranges over the n volume using its physical definition.
 *
 * @code
 * FOR_ALL_DIRECT_NZYX_ELEMENTS_IN_MULTIDIMARRAY(v)
 * {
 *     std::cout << DIRECT_NZYX_ELEM(v,l, k, i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_NZYX_ELEMENTS_IN_MULTIDIMARRAY(V) \
    for (long int l=0; l<NSIZE(V); l++) \
        for (long int k=0; k<ZSIZE(V); k++) \
            for (long int i=0; i<YSIZE(V); i++)      \
                for (long int j=0; j<XSIZE(V); j++)

/** For all direct elements in the array
 *
 * This macro is used to generate loops for the array in an easy
 * manner. It defines internal indexes 'l', 'k','i' and 'j' which
 * ranges over the n volume using its logical definition.
 *
 * @code
 * FOR_ALL_NZYX_ELEMENTS_IN_MULTIDIMARRAY(v)
 * {
 *     std::cout << NZYX_ELEM(v,l, k, i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_NZYX_ELEMENTS_IN_MULTIDIMARRAY(V) \
    for (long int l=0; l<NSIZE(V); l++) \
        for (long int k=STARTINGZ(V); k<=FINISHINGZ(V); k++) \
            for (long int i=STARTINGY(V); i<=FINISHINGY(V); i++)     \
                for (long int j=STARTINGX(V); j<=FINISHINGX(V); j++)

/** For all direct elements in the array, pointer version
 *
 * This macro is used to generate loops for the array in an easy manner. It
 * defines an internal index 'k' which goes over the slices and 'n' that
 * goes over the pixels in each slice. Each element can be accessed through
 * an external pointer called ptr.
 *
 * @code
 * T* ptr=NULL;
 * long int n;
 * FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY_ptr(v,n,ptr)
 * {
 *     std::cout << *ptr << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY_ptr(v,n,ptr) \
    for ((n)=0, (ptr)=(v).data; (n)<NZYXSIZE(v); ++(n), ++(ptr))

/** Access to a direct element.
 * v is the array, k is the slice (Z), i is the Y index and j is the X index.
 */
#define DIRECT_A3D_ELEM(v,k,i,j) ((v).data[(k)*YXSIZE(v)+((i)*XSIZE(v))+(j)])

/** A short alias for the previous function.
 *
 */
#define dAkij(V, k, i, j) DIRECT_A3D_ELEM(V, k, i, j)

/** Volume element: Logical access.
 *
 * @code
 * A3D_ELEM(V, -1, -2, 1) = 1;
 * val = A3D_ELEM(V, -1, -2, 1);
 * @endcode
 */
#define A3D_ELEM(V, k, i, j) \
    DIRECT_A3D_ELEM((V),(k) - STARTINGZ(V), (i) - STARTINGY(V), (j) - STARTINGX(V))

/** For all elements in the array.
 *
 * This macro is used to generate loops for the volume in an easy way. It
 * defines internal indexes 'k','i' and 'j' which ranges the volume using its
 * mathematical definition (ie, logical access).
 *
 * @code
 * FOR_ALL_ELEMENTS_IN_ARRAY3D(V)
 * {
 *     std::cout << V(k, i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_ELEMENTS_IN_ARRAY3D(V) \
    for (long int k=STARTINGZ(V); k<=FINISHINGZ(V); k++) \
        for (long int i=STARTINGY(V); i<=FINISHINGY(V); i++) \
            for (long int j=STARTINGX(V); j<=FINISHINGX(V); j++)

/** For all direct elements in the array.
 *
 * This macro is used to generate loops for the volume in an easy way. It
 * defines internal indexes 'k','i' and 'j' which ranges the volume using its
 * physical definition.
 *
 * @code
 * FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY3D(V)
 * {
 *     std::cout << DIRECT_A3D_ELEM(m, k, i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY3D(V) \
    for (long int k=0; k<ZSIZE(V); k++) \
        for (long int i=0; i<YSIZE(V); i++) \
            for (long int j=0; j<XSIZE(V); j++)

/** Access to a direct element of a matrix.
 * v is the array, i and j define the element v_ij.
 *
 * Be careful because this is physical access, usually matrices follow the C
 * convention of starting index==0 (X and Y). This function should not be used
 * as it goes against the vector library philosophy unless you explicitly want
 * to access directly to any value in the matrix without taking into account its
 * logical position
 *
 * @code
 * DIRECT_A2D_ELEM(m, 0, 0) = 1;
 * val = DIRECT_A2D_ELEM(m, 0, 0);
 * @endcode
 */
#define DIRECT_A2D_ELEM(v,i,j) ((v).data[(i)*(v).xdim+(j)])

/** Short alias for DIRECT_A2D_ELEM
 */
#define dAij(M, i, j) DIRECT_A2D_ELEM(M, i, j)

/** Matrix element: Logical access
 *
 * @code
 * A2D_ELEM(m, -2, 1) = 1;
 * val = A2D_ELEM(m, -2, 1);
 * @endcode
 */
#define A2D_ELEM(v, i, j) \
    DIRECT_A2D_ELEM(v, (i) - STARTINGY(v), (j) - STARTINGX(v))

/** TRUE if both arrays have the same shape
 *
 * Two arrays have the same shape if they have the same size and the same
 * starting point. Be aware that this is a macro which simplifies to a boolean.
 */
#define SAME_SHAPE2D(v1, v2) \
    (XSIZE(v1) == XSIZE(v2) && \
     YSIZE(v1) == YSIZE(v2) && \
     STARTINGX(v1) == STARTINGX(v2) && \
     STARTINGY(v1) == STARTINGY(v2))

/** For all elements in the array
 *
 * This macro is used to generate loops for the matrix in an easy way. It
 * defines internal indexes 'i' and 'j' which ranges the matrix using its
 * mathematical definition (ie, logical access).
 *
 * @code
 * FOR_ALL_ELEMENTS_IN_ARRAY2D(m)
 * {
 *     std::cout << m(i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_ELEMENTS_IN_ARRAY2D(m) \
    for (long int i=STARTINGY(m); i<=FINISHINGY(m); i++) \
        for (long int j=STARTINGX(m); j<=FINISHINGX(m); j++)

/** For all elements in the array, accessed physically
 *
 * This macro is used to generate loops for the matrix in an easy way using
 * physical indexes. It defines internal indexes 'i' and 'j' which ranges the
 * matrix using its physical definition.
 *
 * @code
 * FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY2D(m)
 * {
 *     std::cout << DIRECT_A2D_ELEM(m, i, j) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY2D(m) \
    for (long int i=0; i<YSIZE(m); i++) \
        for (long int j=0; j<XSIZE(m); j++)

/** Vector element: Physical access
 *
 * Be careful because this is physical access, usually vectors follow the C
 * convention of starting index==0. This function should not be used as it goes
 * against the vector library philosophy unless you explicitly want to access
 * directly to any value in the vector without taking into account its logical
 * position.
 *
 * @code
 * DIRECT_A1D_ELEM(v, 0) = 1;
 * val = DIRECT_A1D_ELEM(v, 0);
 * @endcode
 */
#define DIRECT_A1D_ELEM(v, i) ((v).data[(i)])

/** A short alias to previous function
 */
#define dAi(v, i) DIRECT_A1D_ELEM(v, i)

/** Vector element: Logical access
 *
 * @code
 * A1D_ELEM(v, -2) = 1;
 * val = A1D_ELEM(v, -2);
 * @endcode
 */
#define A1D_ELEM(v, i) DIRECT_A1D_ELEM(v, (i) - ((v).xinit))

/** For all elements in the array
 *
 * This macro is used to generate loops for the vector in an easy manner. It
 * defines an internal index 'i' which ranges the vector using its mathematical
 * definition (ie, logical access).
 *
 * @code
 * FOR_ALL_ELEMENTS_IN_ARRAY1D(v)
 * {
 *     std::cout << v(i) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_ELEMENTS_IN_ARRAY1D(v) \
    for (long int i=STARTINGX(v); i<=FINISHINGX(v); i++)

/** For all elements in the array, accessed physically
 *
 * This macro is used to generate loops for the vector in an easy way using
 * physical indexes. It defines internal the index 'i' which ranges the vector
 * using its physical definition.
 *
 * @code
 * FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY1D(v)
 * {
 *     std::cout << DIRECT_A1D_ELEM(v, i) << " ";
 * }
 * @endcode
 */
#define FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY1D(v) \
    for (long int i=0; i<v.xdim; i++)



// ======================== From FFTW.h =====================================

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
#define FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(V) \
    for (long int k = 0, kp = 0; k<ZSIZE(V); k++, kp = (k < XSIZE(V)) ? k : k - ZSIZE(V)) \
    	for (long int i = 0, ip = 0 ; i<YSIZE(V); i++, ip = (i < XSIZE(V)) ? i : i - YSIZE(V)) \
    		for (long int j = 0, jp = 0; j<XSIZE(V); j++, jp = j)

/** For all direct elements in the complex array in FFTW format.
 *  The same as above, but now only for 2D images (this saves some time as k is not sampled
 */
#define FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM2D(V) \
	for (long int i = 0, ip = 0 ; i<YSIZE(V); i++, ip = (i < XSIZE(V)) ? i : i - YSIZE(V)) \
		for (long int j = 0, jp = 0; j<XSIZE(V); j++, jp = j)

/** FFTW volume element: Logical access.
 *
 * @code
 *
 * FFTW_ELEM(V, -1, -2, 1) = 1;
 * val = FFTW_ELEM(V, -1, -2, 1);
 * @endcode
 */
#define FFTW_ELEM(V, kp, ip, jp) \
    (DIRECT_A3D_ELEM((V),((kp < 0) ? (kp + ZSIZE(V)) : (kp)), ((ip < 0) ? (ip + YSIZE(V)) : (ip)), (jp)))

/** FFTW 2D image element: Logical access.
 *
 * @code
 *
 * FFTW2D_ELEM(V, --2, 1) = 1;
 * val = FFTW2D_ELEM(V, -2, 1);
 * @endcode
 */
#define FFTW2D_ELEM(V, ip, jp) \
    (DIRECT_A2D_ELEM((V), ((ip < 0) ? (ip + YSIZE(V)) : (ip)), (jp)))


/** Legacy array to minimally make all the above macros work.
 * This class will have the necessary attributes from the MultiDimArray class
 * in Xmipp and Relion to make these macros work in transition code.
 */

template <class T>
class LegacyArray
{
public:
    // Pointer to data memory
    T * data = nullptr;

    // Number of images
    long int ndim;

    // Number of elements in Z
    long int zdim;

    // Number of elements in Y
    long int ydim;

    // Number of elements in X
    long int xdim;

    // Number of elements in YX
    long int yxdim;

    // Number of elements in ZYX
    long int zyxdim;

    // Number of elements in NZYX
    long int nzyxdim;

    // Z init
    long int zinit;

    // Y init
    long int yinit;

    // X init
    long int xinit;

    LegacyArray(const em::ArrayDim &adim, void * rawMemory):
            ndim(adim.n), zdim(adim.z), ydim(adim.y), xdim(adim.x),
            yxdim(adim.y * adim.x), zyxdim(yxdim * zdim), nzyxdim(zyxdim * ndim),
            zinit(0), yinit(0), xinit(0)
    {
        data = static_cast<T*>(rawMemory);
    }

    LegacyArray(em::Image &image): LegacyArray(image.getDim(), image.getData())
    {}

    /** Set logical origin in Xmipp fashion.
     *
     * This function adjust the starting points in the array such that the
     * center of the array is defined in the Xmipp fashion.
     *
     * @code
     * V.setXmippOrigin();
     * @endcode
     */
        void setXmippOrigin()
        {
            zinit = FIRST_XMIPP_INDEX(zdim);
            yinit = FIRST_XMIPP_INDEX(ydim);
            xinit = FIRST_XMIPP_INDEX(xdim);
        }

}; // class LegacyArray
#endif //EM_CORE_LEGACY_MACROS_H
