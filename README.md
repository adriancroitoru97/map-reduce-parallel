# Parallel processing using MAP-REDUCE paradigm

A program written in `C/C++` using `pthread.h` library, which finds
perfect powers from a large set of input files and counts the unique
values for each exponent.

The **Map-Reduce Google Model** was used.


## Map-Reduce Paradigm

The input files are efficiently assigned to multiple `mappers`,
which verify for each number the perfect power attribute.
**Each mapper builds partial lists for each exponent**.
More precisely, each mapper has lists with perfect squares, perfect cubes etc.

The second part of the program is then handled by the `reducers`.
Their mission is to take the partial lists for each exponent,
from every mapper, and aggregate them all. The final goal is to count
the number of unique values for each exponent from all files.


## Perfect Powers Algorithm

Values lower than 1 are ignored.\
1 is considered a perfect power for every exponent.

For any other number, every possible exponent is checked,
using `binary search algorithm` for finding the suitable base.
If a `base` is found, it means that the number is a
perfect power of `exponent`. In that case,
the number is added in the according partial list.


## Application Flow

A `files queue` is initialized in the beginning.
It is used by mappers to **efficiently balance the load**.

The `partial lists` are hold in a matrix of `unordered_set`.
The first parameter of a partial list is the mapper,
while the second one is the exponent.
The unordered sets are used because of
the uniqueness of the elements contained.

`Mappers` and `Reducers` threads are then created.

Each `mapper` works as long as there are files to be analyzed in the queue.
When a file is done, an available one is opened and the process continues.

Each `reducer` has an assigned exponent. For that exponent,
it takes the partial lists from all mappers and creates a final aggregate list,
an unordered set containing only the unique elements.
The size of this list is the final answer for that exponent,
which is then written in the according output file.

`Synchronization` and `collision avoidance` were ensured
by using a `barrier` and a `mutex`.


## License
[Adrian-Valeriu Croitoru](https://github.com/adriancroitoru97)