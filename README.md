# Inventory Management System (SO Project)

## Overview

This project is an Inventory Management System developed as part of a second-year Operating Systems course.  It simulates a simplified stock control system, allowing users to manage articles, their prices, and stock levels. The system employs inter-process communication (IPC) mechanisms, file handling, and basic data management principles within a Unix-like environment.

## Features

*   **Article Management:**
    *   Adding new articles with names and prices.
    *   Modifying existing article prices.
    *   Changing article names. (This is implemented by writing to a temporary file then sorting it)

*   **Stock Control:**
    *   Viewing current stock levels for articles.
    *   Adding or removing stock from articles.
    *   Sales recording.

*   **Sales Reporting:**
    *   Generating daily sales reports with article quantities and prices.
    *   Calculating total revenue generated per article.

*   **Inter-Process Communication:**
    *   Utilizes FIFOs (named pipes) for communication between client and server processes.
    *   Uses pipes to communicate between server and child process.

*   **Data Persistence:**
    *   Stores article information, stock levels, and sales data in persistent files.

## Architecture

The system is based on a client-server architecture with the following components:

*   **`ma.c` (Manager Application):** The main application for managing articles and prices.  It allows users to:
    *   Insert new articles with names and prices.
    *   Modify existing article prices.
    *   Change existing article names.
    *   Displays the current stock code.
    *   It writes the articles code and prices into file "artigos"
    *   It writes the article names into file "strings"

*   **`sv.c` (Server):** The central server process that handles client requests for stock management and sales reporting. The server:
    *   Listens for client connections via a FIFO.
    *   Handles requests to update stock levels.
    *   Generates sales reports.
    *   Uses pipes to communicate with child process
    *   Creates FIFOs to communication with the client

*   **`cv.c` (Client):** A command-line client that allows users to interact with the server to perform stock operations.  The client:
    *   Connects to the server via a FIFO.
    *   Sends requests to update stock levels and view stock information.

*   **Data Files:**
    *   `artigos`: Stores article codes and prices.
    *   `strings`: Stores article names.
    *   `stock`: Stores current stock levels for each article.
    *   `vendas`: Stores sales transactions (article code, quantity, price).
    *   `precos`: Stores the sales for each item
    *   `quantidade`: Stores the amount sold for each time

## File Descriptions

*   **`ag.c`:** A small program to signal the server to create a report.

*   **`create_test_file.c`:**  A utility program to create test data files for the system, used during development and testing.

*   **`extra.c` and `extra.h`:** Utility functions, including:
    *   `numDigits`: Calculates the number of digits in an integer.
    *   `int2code`: Converts an integer to a fixed-size string (e.g., for article codes).
    *   `spacecount`: Counts the spaces in a string.
    *   `retArg`: Tokenizes a string into an array of arguments.
    *   `cRetArg`: Frees memory allocated for the argument array.

*   **`ma.h`:** Header file for `ma.c`, defining constants and function prototypes.

*   **`Makefile`:** Build automation file for compiling the project.

## Compilation and Execution

1.  **Clone the repository:**

    ```bash
    git clone <repository_url>
    cd <project_directory>
    ```

2.  **Compile the project:**

    ```bash
    make
    ```

    This will compile all the C files using `gcc` and create the executables: `ma`, `sv`, `cv`, and `ag`.

3.  **Run the server:**

    ```bash
    ./sv
    ```

    The server needs to be running before the client.

4.  **Run the manager application:**

    ```bash
    ./ma
    ```

    The manager allow us to manage articles and prices

5.  **Run the client:**

    ```bash
    ./cv
    ```

    The client is a command-line interface for interacting with the server.

6.  **Run the report generator:**

    ```bash
    ./ag
    ```

    It force the server to generate a report

## Usage

### Manager Application (`ma`)

The manager application accepts commands via standard input.  Each command should be on a new line.

*   **Insert a new article:**

    ```
    i <article_name> <price>
    ```

    Example:

    ```
    i coffee 2.50
    ```

*   **Modify an existing article price:**

    ```
    p <article_code> <new_price>
    ```

    Example:

    ```
    p 0000000000 2.75
    ```

*   **Change an article name:**

    ```
    n <article_code> <new_name>
    ```

    Example:

    ```
    n 0000000000 blackcoffee
    ```

### Client (`cv`)

The client accepts commands via standard input. Each command should be on a new line.

*   **View stock level for an article:**

    ```
    <article_code>
    ```

    Example:

    ```
    0000000000
    ```

*   **Add/Remove stock from an article:**

    ```
    <article_code> <quantity_change>
    ```

    Example:

    ```
    0000000000 10  (Add 10 to stock)
    0000000000 -5  (Remove 5 from stock)
    ```

### Report Generator (`ag`)
Run the command on a new terminal.

*   **To generate a report:**
    ```
    ./ag
    ```

## Example Workflow
1. In terminal 1, start the server with ``./sv``
2. In terminal 2, start the manager application with ``./ma``
3. In terminal 3, start the client with ``./cv``
4. Using the manager application, create a new article with ``i coffee 2.50`` this will return the new article code.
5. Using the client application, change the amount of stock of the created article by using the article code and the new amount, example ``0000000000 10``

## Notes

*   Error handling is basic.  The system provides simple error messages to standard output.
*   The file paths for data storage are hardcoded.
*   This project is a simplified model and may not be suitable for production environments without further development.
*   The stock file needs to have as many items as the articles file has.
*   The report is created on the same folder as the executables

## Potential Improvements

*   Implement more robust error handling.
*   Allow configuration of file paths via command-line arguments.
*   Add user authentication and authorization.
*   Implement a more sophisticated reporting system.
*   Use database for data storage.
*   Add more commands to the client application
