# Lucky-Pasta

Lucky-Pasta is a lightweight shellcode loader designed with an emphasis on stealth and evasion techniques. It incorporates runtime protections such as just-in-time decryption, string obfuscation, and dynamic library loading to reduce static and behavioral detection. The loader executes payloads using fibers and applies runtime patching of AES-related CPU instructions to further limit analysis visibility. It supports secure retrieval of shellcode over HTTP and HTTPS, enabling flexible and controlled delivery.


## Features

- JIT Decryption of the shellcode avoiding AV detection of the payload
- Obfuscation of suspicious strings
- Dynamic loading of suspicious libraries
- Shellcode execution using fibers
- Runtime patching of AES CPU instructions to avoid static detection
- Retrieves shellcode with http or https 

## Usage

### Build

Compile the project using MinGW:

```bash
make all
````

This generates two binaries:

* `payloadEncryptor.exe` – encrypts your shellcode
* `filezilla.exe` – the loader

### Configure Payload URL

By default, the loader retrieves the encrypted payload from:

```
http://127.0.0.1:8000/payload.bin.enc
```

To change this, edit the URL in:

```
inc/lucky_http.h
```

### Encrypt Shellcode

Encrypt your payload before serving it:

```bash
wine payloadEncryptor.exe payload.bin payload.bin.enc
```

### Host the Payload

Serve the encrypted payload over HTTP or HTTPS. For example:

```bash
python3 -m http.server
```

### Execute Loader

Run the loader to fetch and execute the payload.



## Notes

- The Code is not badly written and uncommented. It is Red-Wine-Obfuscated™
- Yes payloadEncryptor is an exe. I am sorry
- The Loader has between 0 and 2 Detections on VirusTotal. If you have a detection from Microsft "Trojan:Win32/Wacatac.B!ml" change the loader and compiler flags randomly. Avoiding ML based detections seems more like gambling than science or is for smarter people than me. 



