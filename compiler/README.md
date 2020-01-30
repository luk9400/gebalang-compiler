Autor: Łukasz Bratos 244751

# Kompilator

Kompilator prostego języka imperatywnego napisany na potrzeby kursu "Języki Formalne i Techniki Translacji". Język, asembler oraz maszyna wirtualna są autorstwa dr Macieja Gębali.

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

## Opis załączonych plików
- `code.cpp` i `code.hpp` - klasa generująca kod w asemblerze
- `data.cpp` i `data.hpp` - klasa służąca do zarządzania tablicą symboli
- `symbol.hpp` - definicja struktury przechowującej symbol
- `labels.hpp` - definicje etykiet dla instrukcji warunkowych i pętli
- `lexer.l` - lekser języka wejściowego
- `parser.ypp` - parser języka wejściowego
- `makefile` - plik Makefile do kompilacji

## Adnotacja

~~Podczas testów okazało się, że kompilator dla testu `4-fib-factorial.imp` generuje program, który na standardowej maszynie wirtualnej zapętla się dla argumentów większych niż 64. 
Jednak po zamianie linijki 15. z `s(i) ASSIGN s(j) TIMES m;` na `s(i) ASSIGN m TIMES s(j);` program nie zapętla się i zwraca wynik.
Na maszynie cln zwraca poprawny wynik dla obu wersji testu dla dowolnego poprawnego argumentu.~~

Powyższe rozwiązano wprowadzając optymalizację polegającą na mnożeniu mniejszej liczby przez większą.

Gdy możliwe jest użycie niezainicjalizowanej zmiennej kompilator ostrzeże o tym i skompiluje program. Powoduje to, że testy `error3` i `error5` się kompilują, lecz pokazują ostrzeżenie.