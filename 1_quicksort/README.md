# Практическое задание #1.

Нужно реализовать `quicksort`. От Вас требуется написать последовательную версию алгоритма (`seq`) и параллельную версию (`par`). Взять случайный массив из `10^8` элементов и отсортировать. (Усреднить по 5 запускам) Сравнить время работы `par` на 4 процессах и `seq` на одном процессе - у Вас должно быть раза в 3 быстрее.

# Результаты запусков.

```bash
> make clean && make run
rm -f main
/opt/opencilk/bin/clang++ -fopencilk -ltbb -O3 -Werror -Wall -Wextra -pedantic main.cpp -o main
CILK_NWORKERS=4 ./main
      Sequential | avg: 7369 ms, min: 7356 ms, max: 7378 ms
 Parallel (Cilk) | avg: 2355 ms, min: 2350 ms, max: 2362 ms
Parallel (Cilk+) | avg: 2137 ms, min: 2133 ms, max: 2141 ms
  Parallel (TBB) | avg: 3445 ms, min: 3441 ms, max: 3452 ms
 Parallel (TBB+) | avg: 1766 ms, min: 1765 ms, max: 1769 ms
             std | avg: 6802 ms, min: 6762 ms, max: 6836 ms
```
