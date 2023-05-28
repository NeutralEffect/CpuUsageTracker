/**
 * \file circbuf_types.h
 * Circular buffer public interface.
*/
#ifndef CIRCBUF_H_INCLUDED
#define CIRCBUF_H_INCLUDED
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * Circular buffer handle type, used in every operation on circular buffer.
*/
typedef struct CircularBuffer CircularBuffer_t;


/**
 * \brief Create new circular buffer of given capacity and item size in dynamically allocated memory.
 * \param itemSize Size of singular item.
 * \param capacity Maximum amount of items buffer should be able to hold at once.
 * \return Pointer to newly created circular buffer if successful, NULL otherwise.
*/
CircularBuffer_t* CircularBuffer_create(size_t itemSize, uint32_t capacity);


/**
 * \brief Remove all items from given circular buffer.
 * \param self Circular buffer to be cleared.
*/
void CircularBuffer_clear(CircularBuffer_t* self);


/**
 * \brief Safely remove given circular buffer releasing all of it's resources
 * and deallocating memory used.
 * \param self Circular buffer to be destroyed.
*/
void CircularBuffer_destroy(CircularBuffer_t* self);


/**
 * \brief Get maximum item capacity of given circular buffer.
 * \param self Circular buffer in question.
 * \return Maximum capacity of given buffer.
*/
uint32_t CircularBuffer_getCapacity(const CircularBuffer_t* self);


/**
 * \brief Get amount of items currently held by given circular buffer.
 * \param self Circular buffer in question.
 * \return Currently used capacity of given buffer.
*/
uint32_t CircularBuffer_getItemCount(const CircularBuffer_t* self);


/**
 * \brief Get size of singular item held within given circular buffer.
 * \param self Circular buffer in question.
 * \return Size of items within given circular buffer.
*/
size_t CircularBuffer_getItemSize(const CircularBuffer_t* self);


/**
 * \brief Tests whether given circular buffer is currently empty.
 * \param self Circular buffer in question.
 * \return True if buffer is empty, false otherwise.
*/
bool CircularBuffer_isEmpty(const CircularBuffer_t* self);


/**
 * \brief Tests whether given circular buffer is currently full.
 * \param self Circular buffer in question.
 * \return True if buffer is full, false otherwise.
*/
bool CircularBuffer_isFull(const CircularBuffer_t* self);


/**
 * \brief Read single item without removing it from given circular buffer.
 * \param self Circular buffer in question.
 * \param itemOutPtr Pointer to buffer for element to be written into.
 * \warning Providing buffer that is too small for given item results in undefined behavior.
 * Refer to \ref CircularBuffer_getItemSize(const CircularBuffer_t*) if in doubt.
 * \return True if item has been written into buffer provided by user, false otherwise.
 * Can only fail if buffer is empty or user provided invalid arguments.
*/
bool CircularBuffer_peek(const CircularBuffer_t* self, void* itemOutPtr);


/**
 * \brief Read many items without removing them from given circular buffer.
 * Amount of items read from buffer might be less than the requested amount.
 * \param self Circular buffer in question.
 * \param outputBuffer Buffer to hold items in.
 * \param itemCount Maximum amount of items to be read from circular buffer.
 * \returns Amount of items read from circular buffer.
*/
uint32_t CircularBuffer_peekMany(const CircularBuffer_t* self, void* outputBuffer, uint32_t itemCount);


/**
 * \brief Read single item and remove it from given circular buffer.
 * \param self Circular buffer in question.
 * \param itemOutPtr Pointer to buffer for element to be written into.
 * \warning Providing buffer that is too small for given item results in undefined behavior.
 * Refer to \ref CircularBuffer_getItemSize(const CircularBuffer_t*) if in doubt.
 * \return True if item has been read from circular buffer, false otherwise.
 * Can only fail if buffer is empty or user provided invalid arguments.
*/
bool CircularBuffer_read(CircularBuffer_t* self, void* itemOutPtr);


/**
 * \brief Read many items and remove them from given circular buffer.
 * Amount of items read from buffer might be less than the requested amount.
 * \param self Circular buffer in question.
 * \param outputBuffer Buffer to hold items in. Must be large
 * \param itemCount Maximum amount of items to be read from circular buffer.
 * \returns Amount of items read from circular buffer.
*/
uint32_t CircularBuffer_readMany(CircularBuffer_t* self, void* outputBuffer, uint32_t itemCount);


/**
 * \brief Attempt to write single item into given circular buffer without overwriting existing items.
 * \param self Circular buffer for item to be written into.
 * \param itemPtr Pointer to item that will be written into buffer.
 * \warning Exact amount of bytes copied from user-provided item buffer to internal storage is specified during circular buffer creation and can be checked using \ref CircularBuffer_getItemSize(const CircularBuffer_t*).
 * \return True if item has been written into circular buffer, false otherwise.
*/
bool CircularBuffer_tryWrite(CircularBuffer_t* self, const void* itemPtr);


/**
 * \brief Attempt to write many items into given circular buffer without overwriting existing items.
 * If there isn't enough space in buffer for all the items, not all of them will be written.
 * \param self Circular buffer for items to be written into.
 * \param inputBuffer Array containing items to be written into circular buffer.
 * \param itemCount Amount of items to be written into buffer.
 * \return Amount of items written into buffer.
*/
uint32_t CircularBuffer_tryWriteMany(CircularBuffer_t* self, const void* inputBuffer, uint32_t itemCount);


/**
 * \brief Writes new item to given circular buffer, overwriting old items if necessary.
 * \param self Circular buffer for item to be written to.
 * \param itemPtr Pointer to item to be written into the buffer.
*/
void CircularBuffer_write(CircularBuffer_t* self, const void* itemPtr);


/**
 * \brief Writes many new items to given circular buffer, overwriting old items if necessary.
 * \param self Circular buffer for item to be written to.
 * \param inputBuffer Array containing items to be written into buffer.
 * \param itemCount Amount of items to be written into buffer.
*/
void CircularBuffer_writeMany(CircularBuffer_t* self, const void* inputBuffer, uint32_t itemCount);


#endif // !CIRCBUF_H_INCLUDED
