#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/iface/iface.hpp"

#pragma warning(disable:26495, justification:"" )
InterfaceObject::InterfaceObject()
{
	memset(this->field_0x118, 0, 77 * sizeof(int32_t));
	this->field_0xa0 = -1;
	this->field_0x10c = -1;
	this->field_0x30 = 0;
	this->field_0x2c = 0;
	this->field_0x98 = 0;
	this->field_0x9c = 0;
	this->field_0x18 = 0;
	this->field_0xa4 = 0;
	this->field_0x110 = 0;
	memset(this->field_0xb8, 0, 12 * sizeof(int32_t));
	this->field_0xe8 = 0;
	this->field_0xec = 0;
	this->field_0xf0 = 0;
	this->field_0xf4 = 0;
	this->field_0xf8 = 0;
	this->field_0x108 = 1;
	this->field_0x0c = 0;
	this->field_0x104 = 0;
	this->field_0xb0 = 0;
	this->field_0xac = 1;
	this->field_0xa8 = 10;
	this->field_0xaa = 4;
	this->field_0xae = 500;
}
#pragma warning(default:26495)

void* InterfaceObject::operator new(size_t size)
{
	return g_guiHeap->malloc(size);
}

void* InterfaceObject::operator new(size_t size, void* ptr)
{
	return ptr;
}

void InterfaceObject::operator delete(void* ptr)
{
	g_guiHeap->free(ptr);
}
