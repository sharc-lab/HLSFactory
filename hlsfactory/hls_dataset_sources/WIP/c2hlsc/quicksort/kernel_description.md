Kernel Description:
The quickSort kernel is a high-level synthesis hardware design that implements the quicksort algorithm, a popular sorting algorithm that uses a divide-and-conquer approach to sort an array of integers. The algorithm works by selecting a pivot element from the array and partitioning the other elements into two sub-arrays, according to whether they are less than or greater than the pivot. The sub-arrays are then recursively sorted.

The quickSort kernel takes an array of integers as input and sorts it in-place, meaning that it modifies the original array instead of creating a new one. The kernel uses a recursive approach to sort the array, which allows it to handle arrays of any size.

The algorithm can be described mathematically as follows:
Let $A$ be the input array, and let $n$ be the length of the array. The quicksort algorithm can be defined recursively as:
\[ \text{quicksort}(A, 0, n-1) = \begin{cases}
A & \text{if } n \leq 1 \\
\text{quicksort}(A, 0, p-1) \circ \text{quicksort}(A, p+1, n-1) & \text{otherwise}
\end{cases} \]
where $p$ is the partition index, which is chosen such that all elements in the sub-array $A[0..p-1]$ are less than or equal to the pivot, and all elements in the sub-array $A[p+1..n-1]$ are greater than the pivot.

The kernel uses a partition function to find the partition index, which is then used to recursively sort the sub-arrays. The partition function works by iterating through the array from both ends, swapping elements that are on the wrong side of the pivot.

The kernel has a time complexity of $O(n \log n)$ on average, making it suitable for large datasets. However, in the worst case, the time complexity can be $O(n^2)$, which can occur if the pivot is chosen poorly.

---

Top-Level Function: `quickSort`

Complete Function Signature of the Top-Level Function:
`void quickSort(int arr[], int low, int high);`

Inputs:
- `arr`: the input array to be sorted, which is an array of integers.
- `low`: the starting index of the sub-array to be sorted.
- `high`: the ending index of the sub-array to be sorted.

Outputs:
- None, as the kernel sorts the array in-place.

Important Data Structures and Data Types:
- `int arr[]`: the input array, which is an array of integers.
- `int low` and `int high`: the starting and ending indices of the sub-array to be sorted, which are integers.

Sub-Components:
- `partition`:
    - Signature: `int partition(int arr[], int low, int high);`
    - Details: the partition function is used to find the partition index, which is then used to recursively sort the sub-arrays. The function works by iterating through the array from both ends, swapping elements that are on the wrong side of the pivot.
- `swap`:
    - Signature: `void swap(int *a, int *b);`
    - Details: the swap function is used to swap two elements in the array. It takes two pointers to integers as input and swaps the values of the two integers.