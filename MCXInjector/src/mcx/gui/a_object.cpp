#include "mcx/gui/a_object.h"
#include "mcx_inject.hpp"
#include <cassert>

// __thiscall long x()
static long __fastcall aObject_x(void* thisPtr, void* _) noexcept
{
    return aObject::fromPtr(thisPtr)->x;
}

// __thiscall long y()
static long __fastcall aObject_y(void* thisPtr, void* _) noexcept
{
    return aObject::fromPtr(thisPtr)->y;
}

//static AutoVTablePatchRegister reg_aObject(
//    "aObject", 
//    0x0077ADD4,
//    {
//        {  0, nullptr }, // vector_deleting_destructor
//        {  1, nullptr }, // init
//        {  2, nullptr }, // destroy
//        {  3, nullptr }, // FUN_0060f4c0
//        {  4, nullptr }, // height
//        {  5, (void*)&aObject_x }, // x
//        {  6, (void*)&aObject_y }, // y
//        {  7, nullptr }, // moveTo
//        {  8, nullptr }, // resize
//        {  9, nullptr }, // frame
//        { 10, nullptr }, // addChild
//        { 11, nullptr }, // removeChild
//        { 12, nullptr }, // setParent
//        { 13, nullptr }, // numberOfChildren
//        { 14, nullptr }, // globalX
//        { 15, nullptr }, // globalY
//        { 16, nullptr }, // draw
//        { 17, nullptr }, // findObject
//        { 18, nullptr }, // pointInside
//        { 19, nullptr }, // rectIntersect
//        { 20, nullptr }, // rectIntersect (overload)
//        { 21, nullptr }, // handleEvent
//        { 22, nullptr }, // setBackground
//        { 23, nullptr }, // setBackground (overload)
//        { 24, nullptr }, // setAnimation
//        { 25, nullptr }, // startAnimation
//        { 26, nullptr }, // stopAnimation
//        { 27, nullptr }, // background
//        { 28, nullptr }, // animation
//        { 29, nullptr }, // icon
//        { 30, nullptr }, // setDepth
//        { 31, nullptr }, // children
//        { 32, nullptr }, // FUN_0060ef20
//        { 33, nullptr }, // foremostChild
//        { 34, nullptr }, // depth
//        { 35, nullptr }, // child
//        { 36, nullptr }, // dragStartX
//        { 37, nullptr }, // dragStartY
//        { 38, nullptr }, // dragging
//        { 39, nullptr }, // startDrag
//        { 40, nullptr }, // stopDrag
//        { 41, nullptr }, // FUN_0060e8c0
//        { 42, nullptr }, // leave
//        { 43, nullptr }, // backColor
//        { 44, nullptr }, // setBackColor
//        { 45, nullptr }, // setIcon
//        { 46, nullptr }, // setState
//        { 47, nullptr }, // iconize
//        { 48, nullptr }, // normalize
//        { 49, nullptr }, // maximize
//        { 50, nullptr }, // display
//        { 51, nullptr }, // setPaintRoutine
//        { 52, nullptr }, // setEventRoutine
//        { 53, nullptr }, // paint
//        { 54, nullptr }, // ptr
//        { 55, nullptr }, // startModal
//        { 56, nullptr }, // stopModal
//        { 57, nullptr }, // state
//        { 58, nullptr }, // port
//        { 59, nullptr }, // setDisplayPort
//        { 60, nullptr }, // drawFramed
//        { 61, nullptr }, // drawBox
//        { 62, nullptr }, // drawBox (overload)
//        { 63, nullptr }, // FUN_0060a780
//        { 64, nullptr }, // GetCamera
//        { 65, nullptr }, // IsShowing
//        { 66, nullptr }, // HideMe
//        { 67, nullptr }, // IsHidden
//        { 68, nullptr }, // SetHideDirection
//        { 69, nullptr }, // SetTransparent
//        { 70, nullptr }, // left
//        { 71, nullptr }, // top
//        { 72, nullptr }, // FUN_0060a800
//        { 73, nullptr }, // bottom
//        { 74, nullptr }, // checkSmackerPalette
//        { 75, nullptr }, // getDropTargetCount
//        { 76, nullptr }  // getDropTarget
//    }
//);