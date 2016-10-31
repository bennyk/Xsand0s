
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

    TwoDArray(const TwoDArray &other) : _ysize{other.ysize()}, _xsize{other.xsize()}, _data{nullptr}
    {
        _data = new T[other.length()];
        memcpy(_data, other.data(), other.length() * sizeof(T));
    }

    virtual ~TwoDArray()
    {
        if (_data != nullptr)
            delete _data;
    }

    size_t length()  const { return _xsize * _ysize; }
    const T *data() const { return _data;}
    int xsize() const { return _xsize; }
    int ysize()  const{ return _ysize; }

    void reset(int ysize, int xsize, const T *data)
    {
        if (_data != nullptr)
            delete _data;

        _xsize = xsize; _ysize = ysize;
        size_t sz = xsize * ysize;
        _data = new T[sz];
        memcpy(_data, data, sz * sizeof(T));
    }

    void reset(int ysize, int xsize, T init_val)
    {
        if (_data != nullptr)
            delete _data;

        _xsize = xsize; _ysize = ysize;

        size_t sz = xsize * ysize;
        _data = new T[sz];
        memset(_data, init_val, sz * sizeof(T));
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

    void put(int x, int y, T val) {
        //TODO bad wrap coord calculation hard code!!!
        int x_wrap = x % (int)_xsize;
        x_wrap = x_wrap < 0 ? _xsize + x_wrap : x_wrap;

        int y_wrap = y % (int)_ysize;
        y_wrap = y_wrap < 0 ? _ysize + y_wrap : y_wrap;

        int y_adj = _ysize - y_wrap - 1;

        _data[y_adj * _xsize + x_wrap] = val;
    }

    void print(int extend = 0) const
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

