---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2relaytells.179/"
support_link: "https://www.redguides.com/community/threads/mq2relaytells.66881/"
repository: "https://github.com/RedGuides/MQ2RelayTells"
config: "MQ2Relaytells_<character>_<server>.ini"
authors: "Sadge, brainiac"
tagline: "Will relay any incoming tell a character receives to a chat channel you specify"
---

# MQ2RelayTells

<!--desc-start-->
A plugin that will relay any incoming tell a character receives to a chat channel you specify (default /bc for [MQ2EQBC](../mq2eqbc/index.md)) or email. You can filter messages you don't wish to see with the INI file.
<!--desc-end-->

An optional command-line program, [blat](http://www.blat.net/), is supported to assist with mail / text relay.

## Commands

<a href="cmd-relaycustom/">
{% 
  include-markdown "projects/mq2relaytells/cmd-relaycustom.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relaytells/cmd-relaycustom.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relaytells/cmd-relaycustom.md') }}

<a href="cmd-relayhelp/">
{% 
  include-markdown "projects/mq2relaytells/cmd-relayhelp.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relaytells/cmd-relayhelp.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relaytells/cmd-relayhelp.md') }}

<a href="cmd-relayload/">
{% 
  include-markdown "projects/mq2relaytells/cmd-relayload.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relaytells/cmd-relayload.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relaytells/cmd-relayload.md') }}

<a href="cmd-relaymailhelp/">
{% 
  include-markdown "projects/mq2relaytells/cmd-relaymailhelp.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relaytells/cmd-relaymailhelp.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relaytells/cmd-relaymailhelp.md') }}

<a href="cmd-rtsetkey/">
{% 
  include-markdown "projects/mq2relaytells/cmd-rtsetkey.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relaytells/cmd-rtsetkey.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relaytells/cmd-rtsetkey.md') }}

## Settings

INI file (MQ2RelayTells_character_server.ini) example,

```ini
[Settings]
ChatChannel=/bc
;Defines what channel to relay tells to.
ChannelMessage= sent ${Me} a tell that said:
;Custom message given when relaying tells.
RelayTellsInGame=TRUE
;Toggles if you wish to relay the tells in game or not.
RelayAFKMessage=FALSE
;Toggles the relay of the auto-AFK responses from tells.
RelayTraderTells=FALSE
;Toggles the relay of trader tells.
[Pet]
RelayPetTells=FALSE
;Toggles the relaying of any pet tells.
RelayPetAttack=FALSE
;Toggles the relaying of pet attack messages.
RelayPetSuspend=FALSE
;Toggles the relaying of pet suspend messages.
RelayPetMezzed=FALSE
;Toggles the relaying "Cannot wake" pet messages.
[NPC]
RelayNPCTells=FALSE
;Toggles the relaying of NPC tells
[Mail]
RelayToMail=FALSE
;Toggles the relaying of tells to email.text messages (using blat program).
EmailTo=WhoIsEmailGoingTo@Theiraddress.com
;Destination address for the relayed tell mail.
EmailFrom=EQTellReceived@ingame.com
;Custom address for the sender of the relayed tell mail.
Subject=Tell Received
;Custom subject for the relayed tell mail.
Server=smtp.yourserver.com
;Your smtp server used for sending the mail.
UserName=YourUserName
;Your email user name.
Password=YourPassword
;Your email password.
```

Please note that email with complex modern security, such as GMail, will not work with this plugin.
