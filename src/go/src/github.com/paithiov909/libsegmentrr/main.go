package main

//#include <stdlib.h>
import "C"
import (
	"encoding/json"
	"fmt"
	"github.com/nyarla/go-japanese-segmenter/defaults"
	"github.com/nyarla/go-japanese-segmenter/segmenter"
	"io"
	"strings"
)

//export segment
func segment(x string) *C.char {
	src := strings.NewReader(x)
	dst := new(strings.Builder)
	dict := segmenter.BiasCalculatorFunc(defaults.CalculateBias)
	seg := segmenter.New(dst, src)

	slice := []string{""}

	for {
		err := seg.Segment(dict)
		if err != nil && err != io.EOF {
			panic(err)
		}
		if err == io.EOF {
			break
		}
		slice = append(slice, dst.String())
		dst.Reset()
	}

	slice = append(slice, dst.String())
	dst.Reset()

	data, err := json.Marshal(slice)

	if err != nil {
		fmt.Println("json.Marshal failed:", err)
		s := C.CString(string("ERROR::json.Marshal failed."))
		return s
	} else {
		// fmt.Println("DEBUG Go-side:", string(data))
		s := C.CString(string(data))
		return s
	}
}

func main() {}
