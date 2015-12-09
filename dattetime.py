>>> import os
>>> import datetime
>>> cwd = os.getcwd()
>>> # Time it was last modified
>>> mtime_cwd = os.path.getmtime(cwd)
>>> # Convert time to date
>>> mdate_cwd = datetime.date.fromtimestamp(mtime_cwd)
>>> # Difference in days
>>> dates_old = datetime.date.today() - mdate_cwd
>>> dates_old
datetime.timedelta(13)
>>> dates_old.days
13
