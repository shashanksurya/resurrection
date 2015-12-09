call_count = 0


def count(func):
    def wrapper(*args, **kw):
        global call_count
        call_count += 1
        return func(*args, **kw)
    return wrapper


@count
def hello():
    global call_count
    print(call_count)


if __name__ == '__main__':
    for i in range(5):
        hello()
