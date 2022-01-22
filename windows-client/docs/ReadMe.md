# Arduino Controller, Windows Client

I am not sure if anybody is going to read this but I am trying my best to
document my code so that others might be able to use it.

This page will many serve to list dependencies and anything else that
is not easily put into code documentation.

## Dependencies

- [Boost](https://www.boost.org/) : used for serial, json parsing, date-time calculation, endian and circular buffer

- [OpenCV](https://opencv.org/) : used for image processing

- [SFML](https://www.sfml-dev.org/) : used for inputs

- [FFTW](http://www.fftw.org/) : used for audio processing

- [FFmpeg](https://www.ffmpeg.org/) : used to load video and sound

## Data packets

This section will entail how the data is transfered to the arduino.

The arduino expects there to be 8 bytes sent at a time with 1 byte returned after recieving 8 bytes

### Sent bytes:

|Byte|Mapping|
|-----|---------------|
|1|85 or "U"|
|2|First 8 buttons|
|3|Next 6 buttons|
|4|Left stick x|
|5|Left stick y|
|6|Right stick x|
|7|Right stick y|
|8|Dpad or HAT|

### Button mapping

|Byte|Bit|Mapping|
|-----|-----|----------|
|2|1|Y|
|2|2|B|
|2|3|A|
|2|4|X|
|2|5|L|
|2|6|R|
|2|7|XL|
|2|8|XR|
|3|1|Select|
|3|2|Start|
|3|3|LClick|
|3|4|RClick|
|3|5|Home|
|3|6|Capture|
|3|7|Unused|
|3|8|Unused|

### Dpad mapping

The Dpad is mapped as follows
|Value|Mapping|
|-----|---------------|
|0|Up|
|1|Up + Right|
|2|Right|
|3|Right + Down|
|4|Down|
|5|Down + Left|
|6|Left|
|7|Left + Up|
|8|Neutral|

### Returned Bytes

|Byte|Mapping|
|-----|---------------|
|1|85 or "U"|