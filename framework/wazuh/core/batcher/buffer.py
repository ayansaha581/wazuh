import sys
from typing import List

from wazuh.core.batcher.mux_demux import Message


class Buffer:
    """Manages the buffer for batching messages.

    Parameters
    ----------
    max_elements : int
        Maximum number of messages in the buffer.
    max_size : int
        Maximum size of the buffer in bytes.
    """
    def __init__(self, max_elements: int, max_size: int):
        self.max_elements = max_elements
        self.max_size = max_size
        self._buffer: List[Message] = []

    def add_message(self, msg: Message):
        """Adds a message to the buffer.

        Parameters
        ----------
        msg : Message
            The message to be added to the buffer.

        Returns
        -------
        bool
            `True` if the message was successfully added to the buffer, `False` if adding
            the message would exceed the buffer's limits.
        """
        if self.check_count_limit() or self.check_size_limit():
            return False
        else:
            self._buffer.append(msg)
            return True

    def get_length(self) -> int:
        """Gets the current length of the buffer.

        Returns
        -------
        int
            The number of messages currently in the buffer.
        """
        return len(self._buffer)

    def check_count_limit(self) -> bool:
        """Checks if the buffer has reached the maximum number of messages.

        Returns
        -------
        bool
            True if the buffer has reached the maximum number of messages, False otherwise.
        """
        return self.get_length() >= self.max_elements

    def check_size_limit(self) -> bool:
        """Checks if the buffer has reached the maximum size in bytes.

        Returns
        -------
        bool
            True if the buffer has reached the maximum size, False otherwise.
        """
        total_size = sum(sys.getsizeof(msg.msg) for msg in self._buffer)
        return total_size >= self.max_size

    def copy(self) -> List[Message]:
        """Returns a copy of the buffer.

        Returns
        -------
        List[Message]
            A copy of the list of messages in the buffer.
        """
        return self._buffer.copy()

    def reset(self):
        """Clears the buffer."""
        self._buffer.clear()
