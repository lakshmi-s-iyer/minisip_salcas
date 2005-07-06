/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libminisip/PhoneBook.h>
#include<libminisip/ContactDb.h>

using namespace std;

void PhoneBook::save(){
	if( !io.isNull() ){
		io->save( this );
	}
}

MRef<PhoneBook * > PhoneBook::create( MRef<PhoneBookIo *> io ){
	if( !io.isNull() ){
		MRef<PhoneBook * > phonebook = io->load();
		if( !phonebook.isNull() ){
			phonebook->setIo( io );
		}
		return phonebook;
	}

	return NULL;
}

void PhoneBook::setIo( MRef<PhoneBookIo *> io ){
	this->io = io;
}

void PhoneBook::setName( string name ){
	this->name = name;
}

string PhoneBook::getName(){
	return name;
}

std::list< MRef< PhoneBookPerson * > > PhoneBook::getPersons(){
	return persons;
}

void PhoneBook::addPerson( MRef< PhoneBookPerson * > person ){
	person->setPhoneBook( this );
	persons.push_back( person );
}

void PhoneBook::delPerson( MRef< PhoneBookPerson * > person ){
	list< MRef< PhoneBookPerson * > >::iterator i;
	
	for( i = persons.begin(); i != persons.end(); i++ ){
		if( *(*i) == *person ){
			i = persons.erase( i );
		}
	}
}

string PhoneBook::getPhoneBookId(){
	return io->getPhoneBookId();
}

PhoneBookPerson::PhoneBookPerson( std::string name ):name(name){
}

PhoneBookPerson::~PhoneBookPerson(){
	if( entries.size() != 0 ){
		list< MRef<ContactEntry *> >::iterator i;
		
		for( i = entries.begin(); i != entries.end(); i++ ){
			delEntry( *i );
		}
	}

	if( !phoneBook.isNull() ){
		phoneBook->delPerson( this );
	}
}

void PhoneBookPerson::setName( string name ){
	this->name = name;
}

string PhoneBookPerson::getName(){
	return name;
}

void PhoneBookPerson::addEntry( MRef<ContactEntry *> entry ){
        entry->personIndex = entries.size();
	entries.push_back( entry );
}

void PhoneBookPerson::delEntry( MRef<ContactEntry *> entry ){
	list< MRef< ContactEntry * > >::iterator i;
        uint32_t index = 0;
	
	for( i = entries.begin(); i != entries.end(); i++ ){
		if( *(*i) == *entry ){
			i = entries.erase( i );
		}
                        (*i)->personIndex = index;
                        index++;
	}

	if( entries.size() == 0 ){
		phoneBook->delPerson( this );
	}
}

std::list< MRef<ContactEntry *> > PhoneBookPerson::getEntries(){
	return entries;
}

void PhoneBookPerson::setPhoneBook( MRef<PhoneBook *> phonebook ){
	this->phoneBook = phonebook;
}
