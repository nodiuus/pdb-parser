# pdb-parser
simple windows pdb parser that uses Microsofts DIA SDK

to use this, make sure that msdia140.dll is registered in your windows registry.
if not then run the following in a command prompt with administrative privileges:
```bash
regsvr32 msdia140.dll
```

make sure to also link ```diaguids.lib``` from Visual Studio's DIA SDK folder
