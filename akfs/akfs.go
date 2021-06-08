package akfs
/*
#include "akfs.h"
#include "event.h"
*/
import "C"
import (
	"log"
)

func PsMonitor(){
	go func() {
		ok := C.ProcessMonitor()
		if ok < 0 {
			log.Println("ProcessMonitor error")
		}
	}()

}

func FileMonitor(){
	go func() {
		ok := C.FileMonitor()
		if ok < 0 {
			log.Println("FileMonitor error")
		}
	}()

}

func NetMonitor(){
	go func() {
		ok := C.NetMonitor()
		if ok < 0 {
			log.Println("NetMonitor error")
		}
	}()

}