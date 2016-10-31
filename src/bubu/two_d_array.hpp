
template <class T>
class TwoDArray
{
private:
    size_t _ysize, _xsize;
    T *_data;

public:
    TwoDArray(size_t ysize, size_t xsize, const T *data)
    : _ysize(ysize), _xsize(xsize)
    {
        size_t sz = xsize * ysize;
        _data = new T[sz];
        memcpy(_data, data, sz);
    }

    virtual ~TwoDArray()
    {
        delete _data;
    }

    T at(int x, int y) const {
        // map lower-left to natural upper-right origin
        int x_wrap = x % (int)_xsize;
        x_wrap = x_wrap < 0 ? _xsize + x_wrap : x_wrap;

        int y_wrap = y % (int)_ysize;
        y_wrap = y_wrap < 0 ? _ysize + y_wrap : y_wrap;

        int y_adj = _ysize - y_wrap - 1;

        return _data[y_adj * _xsize + x_wrap];
    }

    void print()
    {
        // print in lower-left origin order.
        for (int j = _ysize - 1; j >= 0; j--) {
            std::cout << j << " ";
            for (int i = 0; i < _xsize; i++) {
                std::cout << at(i, j);
            }
            std::cout << std::endl;
        }
    }

};

