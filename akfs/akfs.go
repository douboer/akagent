package akfs
/*
#include "akfs.h"
#include "process.h"
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