package com.bluehub.fastmixer.common.utils

fun <T> List<T>.areEqual(anotherList: List<T>): Boolean {
    if (this.size != anotherList.size) return false
    return this.zip(anotherList).all { (x, y) -> x == y }
}

fun <T> MutableList<T>.reInitList(anotherList: List<T>) {
    removeAll { true }
    this.addAll(anotherList)
}
