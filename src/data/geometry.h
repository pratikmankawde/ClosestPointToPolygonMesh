/*
 * Author: pratik
 */

#ifndef DATA_GEOMETRY_H_
#define DATA_GEOMETRY_H_

namespace data {

class Geometry {
	public:
		Geometry();
		virtual ~Geometry();

		/**
		 *
		 * @param a
		 * @param b
		 * @param g
		 * @return
		 */
		int test_function(int a, int b, float g);
		private:
		int myPias;

};

} /* namespace data */

#endif /* DATA_GEOMETRY_H_ */
