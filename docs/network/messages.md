# Client / Server Messages
The payloads herein are described in JSON format but are transmitted in binary form. They also contain a 'messageType' enum
for determining how to deserialize the message on the other end. This field is omitted for the purposes of this document.

## Client -> Server

### AuthenticationRequest
```json
{
  "username": "string",
  "password": "string"
}
```
## Server -> Client

### AuthenticationSuccessful
```json
{
  "username": "string"
}
```

### AuthenticationFailed
```json
{}
```

### AccountLoggedInElsewhere
```json
{}
```