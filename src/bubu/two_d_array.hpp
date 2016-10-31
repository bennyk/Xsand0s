
#include <iomanip>

template <class T>
class TwoDArray
{
private:
    int _ysize, _xsize;
    T *_data;

public:
    TwoDArray() : _ysize{0}, _xsize{0}, _data(nullptr)
    {}

    virtual ~TwoDArray()
    {
        if (_data != nullptr)
            delete _data;
    }

    void reset(int ysize, int xsize, const T *data)
    {
        if (_data != nullptr)
            delete _data;

        _xsize = xsize; _ysize = ysize;
        size_t sz = xsize * ysize;
        _data = new T[sz];
        memcpy(_data, data, sz);
    }

    void reset(int ysize, int xsize)
    {
        if (_data != nullptr)
            delete _data;

        _xsize = xsize; _ysize = ysize;

        size_t sz = xsize * ysize;
        _data = new T[sz];
        memset(_data, 0, sz);
    }

    int xsize() { return _xsize; }

    int ysize() { return _ysize; }

    T at(int x, int y) const {
        // map lower-left to natural upper-right origin
        int x_wrap = x % (int)_xsize;
        x_wrap = x_wrap < 0 ? _xsize + x_wrap : x_wrap;

        int y_wrap = y % (int)_ysize;
        y_wrap = y_wrap < 0 ? _ysize + y_wrap : y_wrap;

        int y_adj = _ysize - y_wrap - 1;

        return _data[y_adj * _xsize + x_wrap];
    }

    void put(int x, int y, T val) {
        //TODO bad wrap coord calculation hard code!!!
        int x_wrap = x % (int)_xsize;
        x_wrap = x_wrap < 0 ? _xsize + x_wrap : x_wrap;

        int y_wrap = y % (int)_ysize;
        y_wrap = y_wrap < 0 ? _ysize + y_wrap : y_wrap;

        int y_adj = _ysize - y_wrap - 1;

        _data[y_adj * _xsize + x_wrap] = val;
    }

    void print(int extend = 0)
    {
        // print in lower-left origin order.

        int starty = _ysize - 1, endy = 0;
        int startx = 0, endx = _xsize - 1;
        if (extend > 0) {
            starty += extend;
            endy -= extend;
            startx -= extend;
            endx += extend;
        }

        for (int j = starty; j >= endy; j--) {
            std::cout << std::setw(2) << j << " ";
            for (int i = startx; i <= endx; i++) {
                std::cout << at(i, j);
            }
            std::cout << std::endl;
        }
    }

};

