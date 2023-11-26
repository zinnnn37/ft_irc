# ft_irc
42Seoul ft_irc 레포입니다

---

## 11.26 SUN

1. 채널에 입장하는 클라이언트의 joinedChannel 목록에 해당 채널이 추가되지 않음
2. 채널 입장 시 해당 채널의 client 목록에 새로 입장한 클라이언트가 추가되지 않음 <br />
=> 어제 작성하신 코드로 못 돌려봐서 코드 합치면 될 수도
3. std::map<std::string, Channel> 말고 std::map<std::string, Channel *>로 쓰기<br />
=> 전자는 복사본 저장 아닌가요??? 포인터로 넣어야 원본 수정할 때 같이 수정될 거 같은데<br />
=> 일단 현재 실행되는 코드는 전부 포인터로 고쳤음<br />
=> 이거 Define.hpp에 _**ChannelMap**_으로 typedef 되어 있으니까 이거 쓰세요
4. Define.hpp에 ERR 메시지랑 매개변수 수정했으니까 레퍼런스 말고 이거 보고 해주세요
=> https://www.alien.net.au/irc/irc2numerics.html 이거 보니까 ERR는 user 이름 안넣어도 되는 것 같아서 전부 뺌 && 메시지 일부 수정

### done
* Command 클래스 Singleton으로 변경함
=> handleCommand()에서 command.cmd() 형식으로 호출하면 됨. 따로 신경 쓸 부분 X<br />
=> 명령어에서 server 변수 필요하면 명령어 호출할 때 this 같이 넘기면 됨
* USER 완성
=> irssi로 접속 가능

---

### doing
* PRIVMSG 작성 중
=> JOIN 시 channel에 client가 제대로 추가 안 됨 + joinedChannel에 channel이 제대로 추가 안 됨 이슈로 client가 channel에 없는 것으로 나와서 채널 전송 불가
=> 개인에게 보내는 건 아직 손 안 댐

어제 작성하신 거 기반으로 코드 짜보려고 했는데 없는 함수도 있고 뭔가 제대로 안 돌아가서 따로 작성했어요
나중에 완성하시면 알려주시든가 해주시길..