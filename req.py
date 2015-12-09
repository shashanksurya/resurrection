import gc
#gc.set_debug(gc.DEBUG_LEAK)    # To debug a leaking program

class Base(object):
    def __del__(self):
        print("Base died")


class Derived(Base):
    def __del__(self):
        print("Derived died")


b = Base()
d = Derived()

b.pointer = d
d.pointer = b

import sys
print(sys.getrefcount(b) - 1)    # Output: 2
print(sys.getrefcount(d) - 1)    # Output: 2

del b
del d