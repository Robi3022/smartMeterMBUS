# smartMeterMBUS
The application convertes a MBUS data package to a JSON output. 
Call: `./Converter MBUSData KEY`

The code and the key are given as ASCII string in HEX notation. Bellow see the example.

With that app a MBUS package of Austrian energy providers can be decoded. Testet with a Salzburg AG payload.


## Hardware
The data is read by a ttyUSB0 interface and given to the Converter by a pyton script. (script will be added soon)

## Needed Libs
`sudo apt install libmbedtls-dev build-utils`

## Build
`make all`

## Code Base Reference 
Many thanks to https://github.com/DomiStyle/esphome-dlms-meter

## Example call of the Application

`./Converter 68FAFA6853FF000167DB08454C5310207B85D18201552100074DCEDEA0CBCD1289D208A14D53D8C4670DA4D7D96B72C0F4B29F164A37053A106EEC4835E81C09ED1D39CA3DC0A15E6BA95505019C7D29F4708B57149EC446853F968A9E5E2BE6711BB64DDC945D2311AA144D499126974D077C4AEC2947883D5EA6ED09990D0331CAB035EE0B6E6C1E9F489A62EB2F4056E30AB62549CE388731566B6D428A343FA27E234163E9FE4F518424590F4E4F042BDA5151AC35993FB9DC06CCA346774C490C1B1FCB264D3389E9886E949D029ECC1864D89B565B948566F9CA395B94D1A7294D1886E17735ABB85C13D3C34C22CE41C87DDE8AF2B2EDCB3526FAD3166872726853FF1101674CF6519903348790B32FA26DA7B677D382B55008528F5EC788C80D98BCA9E6CE198CE52EB209FB8DBFBE27E64611571B8E6CC19C8CF9A58E09564823A28FA63F5F2C67B6B807C0D8E9861CBD236E63571395308420456F28DE0A90C8CEC41DFA46C75858101B7674EC5DA2F581C3165D 636d44365073487a51504e677454434e`

You get on stdout:
```
"meterData": {

	"Timestamp": "2023-08-21T01:20:47Z",

	"Serial-Number": "1ELS0208095185",

	"Device-Name": "ELS1200208095185",

	"VoltageL1": 237.000000,

	"VoltageL2": 237.300003,

	"VoltageL3": 237.699997,

	"CurrentL1": 1.950000,

	"CurrentL2": 0.420000,

	"CurrentL3": 1.340000,

	"ActivePowerPlus": 516.000000,

	"ActivePowerMinus": 0.000000,

	"ActiveEnergyPlus": 1084005.000000,

	"ActiveEnergyMinus": 554134.000000,

	"ReactiveEnergyPlus": 19505.000000,

	"ReactiveEnergyMinus": 703464.000000

},

"BytesRead": 376
```

## Add a service to debian systems
- copy file smartMeter.service to /usr/lib/systemd/system/
- adapt names in file

## Example Python script
- An example python script is places in the project repository-folder ./python
- Also a systemd example is included in the folder to call the script at bootup


