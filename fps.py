import datetime

class FPS:
    def __init__(self):
        self._start = None
        self._end = None
        self._number_of_frames = 0

    def update(self):
        self._number_of_frames += 1

    def elapsed(self):
        return (self._end - self._start).total_seconds()

    def start(self):
        self._start = datetime.datetime.now()
        return self

    def stop(self):
        self._end = datetime.datetime.now()

    def fps(self):
        return self._number_of_frames / self.elapsed()
