#include "NetworkHeader.h" // get this file

void HandleTCPClient(int clientSocket) {
	/* TODO: implement this */
}

int main() {
	/* TODO: Command Line arguments */

	/* TODO: Set up TCP connection */

	/* TODO 
	 * 1) Multitheaded programming (pthreads vs select): 
	 * - pthreads: create a new thread to handle each client (share the same address space)
	 * - select: wait for I/O on multiple sockets
	 * - https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/ (this link
	 * - said that select is much easier)
	 *
	 * 2) When a client asks for data, query the current directory for files (can cache these using a data structure)
	 *
	 * 3) What happens if multiple clients sync at once? 
	 * - This will be a problem if 2 files have the same content (files with different contents wont conflict with each other, I suppose?).
	 * - Are mentioned we cannot write to the memory at the same time with multithreading, but 
	 * I haven't found proof with some Google searches. 
	 * - If we use select, then these problems can be ignored I believe.
	 *
	 * Some additional notes:
	 * - Should we cache file contents in a vector to check for duplicates or read from disk every time? (RAM issue)
	 */

	return 0;
}