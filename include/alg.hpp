#ifndef ALG_HPP
#define ALG_HPP

#include <cmath>

template<class Semi>
class alg_vector {
public:
	alg_vector(Semi x, Semi y): _x(x), _y(y) {
	}

	alg_vector( const alg_vector & ) = default;

	alg_vector & operator=( const alg_vector & ) = default;

	Semi x() const {
		return _x;
	}

	Semi y() const {
		return _y;
	}

	alg_vector operator*(const Semi& scalar) const {
		return alg_vector(this->_x*scalar, this->_y*scalar);
	}

	alg_vector operator+(const alg_vector& nd) const {
		return alg_vector(this->_x+nd._x, this->_y+nd._y);
	}

	alg_vector operator-(const alg_vector& nd) const {
		return alg_vector(this->_x-nd._x, this->_y-nd._y);
	}

private:
	Semi _x;
	Semi _y;
};

template<class Semi>
class point {
public:
  point(Semi x, Semi y): _x(x), _y(y) {
	}

	point( const point & ) = default;

	Semi x() const {
		return _x;
	}

	Semi y() const {
		return _y;
	}

	point operator+(const point& nd) const {
		return point(this->_x+nd.x(), this->_y+nd.y());
  }

	point operator-(const point& nd) const {
		return point(this->_x-nd.x(), this->_y-nd.y());
  }

  point operator+(const alg_vector<Semi>& nd) const {
		return point(this->_x+nd.x(), this->_y+nd.y());
  }

	point operator-(const alg_vector<Semi>& nd) const {
		return point(this->_x-nd.x(), this->_y-nd.y());
  }

	template<class Semi2>
	explicit operator point<Semi2>() const {
		return point<Semi2>(static_cast<Semi2>(_x), static_cast<Semi2>(_y));
	}

private:
  Semi _x;
	Semi _y;
};

template<class Semi>
Semi d_to_2(const point<Semi>& st, const point<Semi>& nd) {
	Semi x_diff = st.x() - nd.x();
	Semi y_diff = st.y() - nd.y();
	return x_diff*x_diff + y_diff*y_diff;
}

template<class Semi>
double abs_to_2(const alg_vector<Semi> & v) {
	return dot(v,v);
}

template<class Semi>
Semi dot(const alg_vector<Semi> & st, const alg_vector<Semi> & nd) {
	return st.x()*nd.x() + st.y()*nd.y();
}


#endif
