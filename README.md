# amb_led_tv_V2

I working again on this project but now i am also finishing it... I want to rewrite all my code so might as well just open a new repo. The other repo is not that important and could be deleted if not needed anymore. This repo will hold the most recent code.


## Real readme

This is meant for the linux (this includes raspberry pi).
Made with openCV 4.x


## Instalation
This repo includes instalation for linux and windows. 

### Specs
- Installs Opencv V4.12.0

#### Windows
- Tested with Visual Studio Community 2022 (64-bit) V17.14.15.


### Windows
Steps:

#### Installing opencv
1. Run opencv installation batch script in the Install folder. This will download the opencv V4.12.0 self extracting archieve. This has the opencv library pre-buildt. It will extract directly into the C:\ disk. This is recommended.

#### Linking opencv directories to project
2. Create a new (empty) C++ project with visual studio if you havent already.
3. Goto Project -> Properties -> (Under: Configuration Properties) VC++ Directories -> Include Directories (click on the ▼ button) -> <Edit...> -> Click the folder icon
4. Add the following directories:
- 
