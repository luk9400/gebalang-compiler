# Kompilator

Kompilator prostego języka imperatywnego napisany na potrzeby kursu "Języki Formalne i Techniki Translacji". Język, asembler oraz maszyna wirtualna są autorstwa dr Macieja Gębali. Specyfikacja języka oraz asemblera znajdują się w pliku `labor4.pdf`.

## Zależności

Kompilator wykorzystuje następujące technologie:
- <b>GNU Make</b> 4.1
- <b>Flex</b> 2.6.4
- <b>Bison</b> 3.0.4
- <b>g++</b> 7.4.0

Został on napisany w języku `C++` w standardzie `C++17`.

## Kompilacja

Program należy skompilować przy pomocy polecenia `make`.

## Uruchamianie

Aby poprawnie uruchomić program należy użyć polecenia:

`./kompilator <nazwa_pliku_wejściowego.imp> <nazwa_pliku_wynikowego.mr>` 

## Opis plików
- `code.cpp` i `code.hpp` - klasa generująca kod w asemblerze
- `data.cpp` i `data.hpp` - klasa służąca do zarządzania tablicą symboli
- `symbol.hpp` - definicja struktury przechowującej symbol
- `labels.hpp` - definicje etykiet dla instrukcji warunkowych i pętli
- `lexer.l` - lekser języka wejściowego
- `parser.ypp` - parser języka wejściowego
- `makefile` - plik Makefile do kompilacji