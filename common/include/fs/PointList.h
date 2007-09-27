/**
 * @file PointList.h
 */

#ifndef POINT_LIST_H__
#define POINT_LIST_H__

#include "types.h"
#include "assert.h"

/**
 * @class PointListElement
 * used to keep track of one item on a list.
 */
template <class ContentType> class PointListElement
{
  protected:

    /**
     * the contain pointer
     */
    ContentType *item_;

    /**
     * the prev-element
     */
    PointListElement *prev_;

    /**
     * the next-element
     */
    PointListElement *next_;

  public:

    /**
     * constructor
     */
    PointListElement ( ContentType *item )
    { item_ = item; prev_ = this; next_ = this; }

    /**
     * destructor
     */
    virtual ~PointListElement() {}

    /**
     * set the next-element
     * @param next the element to set
     */
    virtual void setNext ( PointListElement<ContentType> *next ) { next_ = next; }

    /**
     * set the prev-element
     * @param prev the item to set
     */
    virtual void setPrev ( PointListElement<ContentType> *prev ) { prev_ = prev; }

    /**
     * get the next-element
     * @return the next item
     */
    virtual PointListElement* getNext() { return next_; }

    /**
     * get the prev-element
     * @return the previous item
     */
    virtual PointListElement* getPrev() { return prev_; }

    /**
     * get the item (contain) of the element
     * @return the item
     */
    virtual ContentType* getItem() { return item_; }
};

/**
 * @class PointList
 * a doubly linked list of the list elements, each of which points to a single
 * item on the list.
 */
template <class ContentType> class PointList
{
  private:
    /**
     * the assign-operator
     */
    const PointList<ContentType>& operator = ( const PointList<ContentType>& )
    { return ( *this ); }

  protected:

    /**
     * the first-element of the PointList
     */
    PointListElement<ContentType> *first_;

    /**
     * the last_element of the PointList
     */
    PointListElement<ContentType> *last_;

    /**
     * the length of the list
     */
    uint32 length_;

  public:

    /**
     * constructor
     */
    PointList() { first_ = last_ = 0; length_ = 0; }

    /**
     * destructor
     */
    virtual ~PointList();

  protected:

    /**
     * insert element to the list
     * @param new_list_element the new element to insert
     * @param prev the previous element
     * @param next the next element
     */
    virtual void listElementInsert (
        PointListElement<ContentType> *new_list_element,
        PointListElement<ContentType> *prev,
        PointListElement<ContentType> *next );

    /**
     * return a ListElement if that the entry contained.
     * @param the entry to find
     * @return the element found
     */
    virtual PointListElement<ContentType>* find ( ContentType *entry );

    /**
     * Insert a new entry after the specified entry..
     * @param new_entry the entry to append
     * @param entry the entry to which it should be appended
     */
    virtual void append ( ContentType *new_entry, ContentType *entry );

    /**
     * Insert a new entry before the specified entry..
     * @param new_entry the entry to prepend
     * @param entry the entry to which it should be prepended
     */
    virtual void prepend ( ContentType *new_entry, ContentType *entry );

  public:

    /**
     * check the existance of the input entry
     * @param the entry looking for
     * @return true if it is included
     */
    virtual bool included ( ContentType *entry );

    /**
     * Insert a new entry at the end of the list.
     * @param new_entry the entry to push
     */
    virtual void pushBack ( ContentType *new_entry );

    /**
     * Insert a new entry at the first of the list.
     * @param new_entry the entry to push
     */
    virtual void pushFront ( ContentType *new_entry );

    /**
     * remove the element that the entry contained.
     * @param entry the entry to delete
     * @return 0 on success
     */
    virtual int32 remove ( ContentType *entry );

    /**
     * remove the first element of the list and return the corresponde item
     * @return the first element
     */
    virtual ContentType* popFront();

    /**
     * return the point of ContetnType with the index in the list.
     * @param index the position wanted
     * @return  the entry to which it should be appended
     */
    virtual ContentType* at ( uint32 index );

    /**
     * the []-operator
     * @param index the position wanted
     * @return  the entry to which it should be appended
     */
    virtual ContentType* operator[] ( uint32 index );

    /**
     * check the emptiness of the list
     * @return true if empty
     */
    virtual bool empty();

    /**
     * get the length of the list
     * @return the length
     */
  virtual uint32 getLength() { return length_; }
};


template <class ContentType>
PointList<ContentType>::~PointList()
{}


template <class ContentType>
void PointList<ContentType>::listElementInsert (
    PointListElement<ContentType> *new_list_element,
    PointListElement<ContentType> *prev,
    PointListElement<ContentType> *next )
{
  prev->setNext ( new_list_element );
  new_list_element->setPrev ( prev );
  new_list_element->setNext ( next );
  next->setPrev ( new_list_element );
}


template <class ContentType>
PointListElement<ContentType>* PointList<ContentType>::find ( ContentType *entry )
{
  if ( first_ == 0 )
    return 0;
  PointListElement<ContentType> *at = first_;
  PointListElement<ContentType> *prev;
  do
  {
    if ( at->getItem() == entry )
    {
      return at;
    }
    prev = at;
    at = prev->getNext();
  }
  while ( prev != last_ );
  return 0;
}


template <class ContentType>
bool PointList<ContentType>::included ( ContentType *entry )
{
  if ( first_ == 0 )
    return false;

  PointListElement<ContentType> *at = first_;
  PointListElement<ContentType> *prev;

  do
  {
    if ( at->getItem() == entry )
      return true;
    prev = at;
    at = prev->getNext();
  }
  while ( prev != last_ );

  return false;
}


template <class ContentType>
void PointList<ContentType>::pushBack ( ContentType *new_entry )
{
  length_++;
  PointListElement<ContentType> *new_element =
      new PointListElement<ContentType> ( new_entry );
  if ( first_ == 0 ) // if the list is empty
  {
    first_ = new_element;
    last_ = new_element;
  }
  else
  {
    PointListElement<ContentType> *at = last_;

    at->setNext ( new_element );
    new_element->setPrev ( at );

    last_ = new_element;
  }
}


template <class ContentType>
void PointList<ContentType>::append ( ContentType *new_entry, ContentType *entry )
{
  length_++;
  PointListElement<ContentType> *new_element =
      new PointListElement<ContentType> ( new_entry );
  PointListElement<ContentType> *at;
  at = find ( entry );
  assert ( at != 0 );
  if ( at != 0 )
    listElementInsert ( new_element, at, at->getNext() );
}


template <class ContentType>
void PointList<ContentType>::pushFront ( ContentType *new_entry )
{
  length_++;
  PointListElement<ContentType> *new_element =
      new PointListElement<ContentType> ( new_entry );
  if ( first_ == 0 ) // if the list is empty
  {
    first_ = new_element;
    last_ = new_element;
  }
  else
  {
    PointListElement<ContentType> *at = first_;

    new_element->setNext ( at );
    at->setPrev ( new_element );

    first_ = new_element;
  }
}


template <class ContentType>
void PointList<ContentType>::prepend ( ContentType *new_entry, ContentType *entry )
{
  length_++;
  PointListElement<ContentType> *new_element =
      new PointListElement<ContentType> ( new_entry );
  PointListElement<ContentType> *at;
  at = find ( entry );
  assert ( at != 0 );
  if ( at != 0 )
    listElementInsert ( new_element, at->getPrev(), at );
}


template <class ContentType>
ContentType* PointList<ContentType>::popFront()
{
  assert ( empty() != true );

  length_--;
  PointListElement<ContentType> *element = first_;
  ContentType *item = first_->getItem();
  if ( first_ == last_ )
  {
    // List has only one entry, now has none
    first_ = 0;
    last_ = 0;
  }
  else
    first_ = element->getNext();

  delete element;
  return item;
}


template <class ContentType>
int32 PointList<ContentType>::remove ( ContentType *entry )
{
  if ( length_ == 0 )
    return -1;
  PointListElement<ContentType> *element;
  element = 0;
  element = find ( entry );
  if ( element == 0 )
    return -1;
  if ( length_ == 1 )
  {
    length_ = 0;
    first_ = 0;
    last_ = 0;
    delete element;
  }
  else
  {
    length_--;
    PointListElement<ContentType> *prev = element->getPrev();
    PointListElement<ContentType> *next = element->getNext();
    if ( element == first_ )
    {
      next->setPrev ( next );
      first_ = next;
    }
    else if ( element == last_ )
    {
      prev->setNext ( prev );
      last_ = prev;
    }
    else
    {
      prev->setNext ( next );
      next->setPrev ( prev );
    }
    delete element;
  }
  return 0;
}


template <class ContentType>
bool PointList<ContentType>::empty()
{
  if ( length_ == 0 )
    return true;
  else
    return false;
}


template <class ContentType>
ContentType* PointList<ContentType>::at ( uint32 index )
{
  if ( index >= length_ )
    return 0;

  PointListElement<ContentType> *prev = first_;
  PointListElement<ContentType> *at = prev;
  while ( index != 0 )
  {
    at = prev->getNext();
    prev = at;
    index--;
  }

  ContentType *item = at->getItem();
  return item;
}


template <class ContentType>
ContentType* PointList<ContentType>::operator[] ( uint32 index )
{
  if ( index >= length_ )
    return 0;

  PointListElement<ContentType> *prev = first_;
  PointListElement<ContentType> *at = prev;
  while ( index != 0 )
  {
    at = prev->getNext();
    prev = at;
    index--;
  }

  ContentType *item = at->getItem();
  return item;
}

#endif
