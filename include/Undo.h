///////////////////////////////////////////////////////////////////////////////
// Name:        Undo.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: Undo.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __Undo_h__
#define __Undo_h__

// undo is a template class that maintains a dual undo/redo stack.
// The items on the stack are of type 'Type', user-defined.
// The stack itself is actually an STL vector containing both undo and redo items.
// The variable m_uiWaterline is the indicator of the division between the undo and the redo items.

#include <vector>

namespace Undo
{

template<typename Type>
class Stack
  {
  public:
    Stack() :
			m_uiWaterline(0)
      {
      Clear();
      }

    virtual ~Stack()
      {
      Clear();
      }

    virtual void Clear()
      {
      int nCount = m_v.size();
			if(nCount > 0)
				m_v.clear();
      m_uiWaterline = 0;
      }

    virtual void Add(Type typ)
      {
      // Clear all redo elements
      m_v.erase(m_v.begin() + m_uiWaterline, m_v.end());
      // Add the new element
      m_v.push_back(typ);
      m_uiWaterline++;
      }

    bool CanUndo()
      {
      unsigned int uiSize = m_v.size();
      return (m_uiWaterline > 0) ? true : false;
      }

    bool CanRedo()
      {
      unsigned int uiSize = m_v.size();
      return (m_uiWaterline < uiSize)? true : false;
      }

    Type Undo()
      {
      if(!CanUndo())
				throw "Undo called on empty undo stack";
			
      m_uiWaterline--;
      Type typ = m_v.at(m_uiWaterline);
      return typ;
      }

    Type Redo()
      {
			if(!CanRedo())
				throw "Stack::Redo() called on empty stack";

     	m_uiWaterline++;
      Type typ = m_v.at(m_uiWaterline - 1);
      return typ;
      }

  protected:
    std::vector<Type> m_v;
    unsigned int      m_uiWaterline;
  };

template <typename Type>
class DeletingStack : public Stack<Type>
  {
  // friends, classes, enums and types
	public:
		typedef typename std::vector<Type>::const_iterator ConstantIterator;
	private:
		typedef typename std::vector<Type>::iterator Iterator;

  // Constructors/destructors
  public:
    DeletingStack()
			{}
    virtual ~DeletingStack()
      {
      Clear();
      }

  // Operators
  // Methods
  public:
    virtual void Clear(void)
      {
			for(Iterator it = this->m_v.begin(); it != this->m_v.end(); it++)
				delete *it;
			
      int nCount = this->m_v.size();
			if(nCount > 0)
				this->m_v.clear();
      this->m_uiWaterline = 0;
      }

    virtual void Add(Type typ)
      {
			// Remove items above waterline
			for(Iterator it = this->m_v.begin() + this->m_uiWaterline; it != this->m_v.end(); it++)
				delete *it;

      // Clear all redo elements
      this->m_v.erase(this->m_v.begin() + this->m_uiWaterline, this->m_v.end());
      // Add the new element
      this->m_v.push_back(typ);
      this->m_uiWaterline++;
      }

    virtual void Replace(Type typFind, Type typReplace)
      {
			for(Iterator it = this->m_v.begin(); it != this->m_v.end(); it++)
				{
				if(*it == typFind)
					{
					delete *it;
					*it = typReplace;
					return;
					}
				}

			throw "DeletingStack::Replace() Find item is not in stack";
      }
  };

} // namespace Undo

#endif // ndef __Undo_h__
