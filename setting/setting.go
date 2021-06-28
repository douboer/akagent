package setting

import (
	"crypto/md5"
	"encoding/hex"
	"io"
	"log"
	"os"
)

func HashFileMd5(filePath string) (string, error) {
	var returnMD5String string
	file, err := os.Open(filePath)
	if err != nil {
		return returnMD5String, err
	}
	defer file.Close()
	hash := md5.New()
	if _, err := io.Copy(hash, file); err != nil {
		return returnMD5String, err
	}
	hashInBytes := hash.Sum(nil)[:16]
	returnMD5String = hex.EncodeToString(hashInBytes)
	return returnMD5String, nil
}

var (
	SelfHash	string
	SelfName	string
)

func init(){
	var err error

	SelfHash, err = HashFileMd5(os.Args[0])
	if err == nil {
		log.Println("SelfHash :",SelfHash)
	}

	SelfName = os.Args[0]

}