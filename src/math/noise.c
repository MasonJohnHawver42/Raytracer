
#include <math.h>

#include "math/common.h"
#include "math/vector.h"
#include "math/noise.h"

//orignoal prems used by Ken Perlin for quick hashing
int permutation[] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 
                      103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 
                      26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 
                      87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 
                      77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 
                      46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 
                      187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 
                      198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 
                      255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 
                      170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 
                      172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 
                      104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 
                      241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 
                      157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 
                      93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
                      151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 
                      103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 
                      26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 
                      87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 
                      77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 
                      46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 
                      187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 
                      198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 
                      255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 
                      170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 
                      172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 
                      104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 
                      241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 
                      157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 
                      93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

vec3 grads[] = {{0.6016933700097927, 0.09414653119838653, -0.793159201642123},
                {0.7850814599881871, -0.6105819413343523, 0.10409992362720517},
                {0.1234614693850385, -0.8949517682794733, 0.42874071188858554},
                {0.4643023602133227, 0.08452420629508213, -0.8816342647892743},
                {0.030986603693347124, 0.9829867265246878, 0.18104398876469305},
                {-0.7385054938044927, 0.5318809698955398, -0.4143817919305364},
                {-0.6969602162172983, 0.377067652157717, 0.6099724934016317},
                {-0.08744628858306863, 0.9789902080766612, 0.18420455776951442},
                {0.2597739082117936, -0.18457310115605952, 0.9478661756503424},
                {-0.15724598034007473, -0.4659069055245859, 0.8707493652316913},
                {0.6976453540600734, 0.29368283009785023, -0.6534840130134111},
                {0.9194764202248938, 0.31978151329664495, 0.22869826497838153},
                {-0.9819813417345659, 0.09747345945427739, -0.16189987395670527},
                {-0.3401655677726821, 0.36477894144699363, -0.866731625348181},
                {0.387565016572139, 0.7051810823469099, 0.5937280514086212},
                {-0.33215017287084186, 0.3459335550307831, -0.8774999932567715},
                {0.862600584598639, -0.2014255722697428, 0.464055999084047},
                {0.41672243802639103, 0.1266364664452519, -0.9001697701053959},
                {-0.7845319073728161, 0.3290743028264346, -0.5255661609476652},
                {0.05121402673673645, -0.7183667618317574, -0.6937768509836315},
                {-0.10411749915196761, -0.9273786541929973, 0.35934437259754776},
                {0.6528102682400102, 0.054638163001449296, 0.7555484265248873},
                {0.95887890777504, -0.01515773026417991, 0.2834104504731336},
                {-0.24433190195308477, -0.9612864849997801, -0.12739786279508702},
                {-0.7636394527805966, -0.6451985119818108, 0.023951331766901014},
                {0.7729839332451841, -0.27897006609205915, -0.56979956227554},
                {0.7270651035279102, -0.22268064856423286, 0.6494533578225321},
                {-0.13100841620968326, 0.32349432094630637, 0.9371169719932078},
                {-0.919749079235405, 0.28951114272445616, 0.26503760013251737},
                {0.46031587211980063, -0.5455297110274383, -0.7003617866937824},
                {-0.0911353853330499, -0.19030155123544043, 0.9774863994642491},
                {-0.2616198893182991, -0.5647924966759593, -0.7826649789096337},
                {-0.9608596446243841, 0.10631067331348244, -0.25582569079733997},
                {0.6527216017664316, -0.32987063882698614, 0.6820116364310362},
                {0.1326029200973544, -0.9118862390453717, 0.3884326873749711},
                {-0.566513314934546, -0.8227905564641061, -0.045589079781880044},
                {-0.09603458450474849, 0.25013954524248044, 0.9634352943944318},
                {0.15102945261269896, -0.0015102930495262921, -0.9885281095944682},
                {-0.22598479805841318, -0.27065608476813563, 0.9357756968550113},
                {-0.08566339618746815, 0.9102514374660026, -0.4050976464319495},
                {-0.23351485751887438, 0.20152466343052666, 0.951240569649525},
                {0.41219004997988523, -0.262816844276949, -0.8723684250715896},
                {-0.18408466213096067, 0.9542481801049645, -0.23563371561492072},
                {0.6681807156272193, -0.3179473612132034, -0.6726395816196501},
                {-5.493859211159564e-05, -0.79373808047977, -0.6082596950136029},
                {0.630857024668203, -0.6400491490723804, 0.43858465682066916},
                {0.09215279785962066, 0.8227606878295975, 0.5608678207998845},
                {0.06554343208278779, -0.9404950351898371, -0.33342637462275854},
                {0.3304992710831602, -0.8268770239820131, 0.45502155885644485},
                {-0.013870124656822599, 0.7492107126339829, 0.6621864750328896},
                {-0.7431950139403883, 0.6520991851610924, -0.14975921997120914},
                {-0.11590302701234417, -0.19822818105175935, -0.9732790332511467},
                {-0.749096944123182, -0.013792775444639882, -0.6623167879880775},
                {-0.782741691402422, -0.0014413650572618574, -0.622345054617812},
                {-0.8905989390834177, -0.44512843814311664, 0.09324271156374886},
                {0.010832311938602074, -0.7307820442637107, -0.6825249188123587},
                {0.6071944207173234, -0.787626255556092, -0.10468914465427463},
                {0.060222795019874445, 0.5240124901812708, 0.8495787927520424},
                {-0.36464862934883774, 0.3603091273390133, -0.8586085894458618},
                {-0.5930745747618794, -0.7194435492444268, 0.36147410449657114},
                {0.46819302386828615, -0.7974916398150369, 0.38052907487627513},
                {-0.9505292298602971, -0.08853226559064287, 0.29775194563693175},
                {0.070883936230469, -0.5693331783780464, -0.8190452976376986},
                {0.5338869587849289, -0.020164724966673285, 0.8453153844018209},
                {0.6263267008063066, 0.6080071297696219, -0.48789567942993056},
                {0.9909360194608912, -0.059227080464522575, 0.120573455928962},
                {0.6766882873640273, 0.6300853055815927, 0.38090086562580877},
                {-0.8235317181946964, 0.5458739521385942, -0.15432801917954841},
                {0.5571434615704675, 0.638220426233883, 0.5312869759057884},
                {0.3405897125228533, -0.417726277341705, 0.8423202508202217},
                {0.2144143102676818, 0.7404979816699672, 0.6369373930734004},
                {-0.30063038654337954, -0.7056567601770876, -0.6416150773658257},
                {-0.9131668116332755, 0.09676723624984165, -0.3959324135759603},
                {-0.7820023842770072, -0.5309679002594376, 0.32641286720832513},
                {-0.10858460882957462, 0.9239456157135563, 0.36678860659653206},
                {0.8347737169778056, -0.08255878311437626, -0.5443683392462607},
                {0.1375562592703616, 0.8435329595750959, -0.5191631936549669},
                {-0.051360386172993315, -0.9964273195878183, 0.0670425798369718},
                {-0.31664335807051613, -0.6115063900473013, 0.7251185549419796},
                {-0.7627934855026962, 0.40332756900102906, -0.5054433406014647},
                {0.22514268834273088, 0.9353794525636656, -0.2727197272063482},
                {-0.38867441735714564, 0.780819797956597, -0.48913458312728475},
                {0.0951943723403665, -0.9387152203117691, 0.3312880417850157},
                {0.6024124366327893, 0.42181961262732376, 0.6776189715415886},
                {0.43703008368831026, -0.8357203701950304, -0.332529951722352},
                {-0.7826088423870555, -0.4623150479767762, 0.41687911465054733},
                {0.9077445196261258, 0.023581485173423927, -0.4188601206199388},
                {-0.2068565476115599, -0.9065768665023348, -0.3678705666848521},
                {0.6917638603304429, -0.3167931564348903, -0.6489259261092459},
                {-0.43687268781213345, 0.7659094823506288, -0.4717254704689943},
                {0.28361270421129214, -0.8062938175053006, 0.5190896973189578},
                {-0.4212052004816893, -0.7202463312480751, 0.5512090360388406},
                {-0.07386789969261344, 0.3085497755722358, -0.9483356839164732},
                {-0.9387296814414875, -0.2127197250339011, -0.2711768864823567},
                {-0.08327562565877092, -0.9773278047068608, 0.19466774853069138},
                {0.732337622851459, 0.6800126485311461, -0.03555845882388865},
                {0.8201911721489242, 0.5175251043841654, -0.24383233473256258},
                {0.07011700952849889, -0.5979152532359753, -0.7984866654631998},
                {-0.04862405763571857, 0.8909459585116292, -0.45149861575738287},
                {-0.876625581182894, 0.13902925618758014, 0.4606500367303594},
                {-0.23213429930902318, -0.15975049843522793, -0.9594756095565978},
                {-0.9152941883844739, 0.011125779936560658, -0.4026322959356469},
                {0.6915154221069061, -0.05609629800109416, -0.7201802735002395},
                {0.3186979889764903, -0.44654470854542094, 0.8360797899078927},
                {0.8089543671958427, -0.05869567776348191, -0.5849338844746977},
                {0.4177912214523152, 0.7849977102497945, -0.4574156645546393},
                {-0.376217220946514, -0.867519042846877, 0.3253787223549202},
                {-0.6544308526063167, -0.7074228487308999, 0.26697035837416044},
                {0.8404481133019336, 0.540823815311891, 0.03400837600812112},
                {0.06283688699712414, 0.9675769831799254, -0.24463504910978107},
                {-0.8734553386795901, -0.09486700134108993, 0.47757305555137025},
                {-0.768428143835953, -0.5418013075477576, 0.34054299420245315},
                {-0.9914031345909846, -0.12995148995947456, 0.015245818458930515},
                {0.8602920623255443, 0.4421250607138668, -0.25381685954329575},
                {0.4340904359393908, 0.31393635534187736, -0.8443988738863928},
                {-0.9821945308851585, 0.18677161526587635, -0.02025505443732441},
                {0.8194042739969362, 0.4968967018729419, -0.2857801662525702},
                {-0.3122485268827579, -0.07576545351852552, -0.9469743679280236},
                {-0.824248586802194, 0.5491445645966893, -0.138038090136377},
                {0.7336604690806736, 0.26751465448229567, 0.6246424783390447},
                {-0.7279261874197772, -0.49241854827036624, -0.4771241337302195},
                {0.3592110053542578, 0.7165714312668272, 0.5979070475622394},
                {-0.372560099124844, -0.8537668648306298, -0.3637047058499016},
                {-0.14159056102822598, -0.38981244433151874, 0.9099441583262122},
                {0.9203387193922756, -0.37055811891880347, -0.12515319448877057},
                {-0.19534130538115665, 0.2727449728857116, 0.9420466836508465},
                {0.054902208376504696, 0.9376241612179335, 0.3432880420517872},
                {-0.031129321173302013, -0.7111787399176461, 0.702321696413003},
                {-0.8072115314295563, -0.5035975674907628, -0.3078945169121029},
                {0.4361350512183169, 0.8995562271865989, -0.024182870561786703},
                {0.573186345109812, -0.638727994408554, 0.513306889626938},
                {-0.22605340085516665, -0.956123521334291, 0.1863536206064395},
                {-0.9771560127437396, -0.15593694379462156, -0.14439458548972484},
                {-0.7070402118542851, -0.5910425958016696, 0.38828184192551646},
                {-0.0918769264252563, -0.1315204786197466, -0.9870466017844755},
                {-0.6529378480308777, 0.02191521202598367, -0.7570943733053778},
                {0.8392334726399384, -0.49401529422057505, 0.22723570907071028},
                {0.25673450958919286, 0.4171822448316929, 0.8718063811324084},
                {-0.4163653885065559, 0.21648014278131156, 0.8830493819912721},
                {0.16690767563394154, -0.9859517729212431, -0.006397600169791143},
                {-0.6042645292128241, 0.5361174810716539, 0.5894424698175291},
                {-0.46112476287473386, 0.8116072152313507, 0.35868883624686787},
                {0.42672004115783296, 0.552630422648919, -0.7158977737338862},
                {0.6152576557178042, -0.7562977948929545, 0.2224222617470803},
                {0.1418189731224779, -0.18632294925664553, 0.9721991243787413},
                {-0.12359552837872101, 0.7491765058757672, 0.6507370501274392},
                {0.5120974671527283, 0.2624906128077215, 0.8178354738720908},
                {-0.49920369525548675, 0.6673272279483038, -0.5526934425901032},
                {0.5249798910206753, 0.7039331755683413, -0.4784079831672864},
                {0.7350159764011084, 0.0025518713579682395, -0.6780449855191745},
                {0.9719614176672028, -0.19652866603168362, -0.1291026180763521},
                {0.29052996562735056, 0.04248103904119342, -0.9559224342981762},
                {0.13267441995236393, -0.4737218846642266, 0.8706233825716359},
                {0.5548951128315012, 0.6408793611904537, -0.5304385526673484},
                {0.5077376072928625, -0.7868114827645367, 0.3508991489450922},
                {-0.24246656973947336, 0.8798187938561846, 0.4088139583431828},
                {0.4952323658594325, -0.6297814493842375, -0.5984314746207441},
                {0.7220910868447841, -0.3403320519478079, -0.6022944103312874},
                {-0.5422409352755482, 0.23112494258760571, -0.8078094014217554},
                {-0.43702130290743474, 0.3722187622360639, 0.8188196222884139},
                {0.8550281136007009, -0.21579379758991732, 0.4715505931224652},
                {-0.8510028135086541, 0.5010617269885407, 0.15726206518933866},
                {-0.03281599759065491, 0.9979307350368671, -0.05529338451305143},
                {-0.10919147466725183, -0.9763361300449663, 0.1866681092977834},
                {0.8011338706245257, 0.07794130436413804, 0.5933882998612146},
                {-0.8960266646485427, -0.026059194418254304, -0.4432348526741547},
                {0.8757865882672596, 0.31280287981601046, 0.3676305348988357},
                {0.5646336605062194, -0.5265602864013041, -0.6355494427724124},
                {0.8245821375200153, -0.42559092393192877, -0.37274208770903455},
                {-0.7832023626787542, 0.6085830128258899, 0.12736080870574001},
                {0.31163369964562565, -0.9340200887143041, -0.17461646864858849},
                {0.023790394874770363, 0.32583614918993825, 0.9451268808963031},
                {0.4414393695624341, 0.3927850305939261, -0.8067534956488563},
                {0.5593467125848952, -0.8156161430652505, -0.14799176427029762},
                {-0.7981144300866624, -0.17326692514619838, 0.5770545287386832},
                {0.11048563575978794, -0.9923614339149706, -0.054879037609809424},
                {0.7550400727613468, -0.39246313131678345, -0.5252496350132625},
                {-0.9795786823513648, 0.052026433598626444, 0.19421342716138787},
                {-0.9564244795521469, 0.26508343983044325, 0.12240500333346152},
                {0.10924437035256238, -0.8466519661920368, -0.5208129373291627},
                {-0.13203950067321032, 0.6253327871872246, 0.7691062836374609},
                {0.4313284313207319, 0.5966480701302765, -0.676732491272743},
                {0.885794650181664, -0.4587175617385572, 0.07032806169784893},
                {0.046357959623826606, 0.5621484118996545, 0.825736097417457},
                {0.03340650839783689, 0.027641697329294813, -0.9990595286395205},
                {-0.34523020432222096, 0.5642682132039472, -0.7499449910435181},
                {0.5359648235784891, 0.8086420479515184, 0.2425690544386346},
                {-0.4396081227407441, -0.5942044410149308, -0.6735471629355234},
                {-0.8585751280002897, -0.4694628039478452, -0.20604229004918675},
                {-0.3863290300414864, -0.5550649647381425, 0.7366496897898274},
                {0.004429426012172842, -0.6572910873440114, -0.7536237832521807},
                {-0.8391326120765338, -0.23116491920957916, -0.4923608833736108},
                {0.41943681050447545, -0.167475530301141, -0.892202168089825},
                {-0.4533849382493313, 0.8570352580248772, 0.24481148721185889},
                {0.08178571233816548, 0.0454785479213285, -0.995611771192118},
                {0.3893715048574273, -0.42429021070968764, 0.8175375516152095},
                {-0.5323901005043885, -0.1404832943501956, -0.8347605793839593},
                {0.420211002000078, 0.8557502376120623, 0.30185136180741234},
                {0.336514140136116, -0.8678204875006944, 0.36557603171229325},
                {-0.8188999910067463, -0.5574728016378729, -0.13648032885061356},
                {-0.9851283485315776, 0.019511917061702976, -0.17070858798555993},
                {0.42864210838264405, -0.8655226807950394, 0.25910698938980803},
                {-0.5938730731558794, 0.23893603790714596, -0.7682605956116885},
                {0.17003428565898307, 0.6690946059765474, 0.7234644082147568},
                {0.802371473538289, -0.5918079292197688, 0.07722301059013359},
                {0.14900757644977441, 0.978146512211449, 0.14500394066763272},
                {-0.5556120383079084, -0.271688935147577, -0.7857992017084944},
                {-0.9169337511069412, -0.0778596446221523, 0.3913698657539518},
                {0.7612090844423204, -0.42857021974402365, 0.48671171807451236},
                {-0.2020761499818646, 0.9765114160398266, -0.07477087636506463},
                {-0.7549220202419056, 0.035675671692836995, -0.6548434849665566},
                {0.08192886333216721, -0.6868159113250277, 0.722199117490371},
                {0.7518185418705994, 0.6590569087066268, -0.020319231915371636},
                {0.14334707241040257, -0.398277081605685, 0.9059950237716681},
                {0.40535275893831846, 0.8317794481071723, -0.3792520145333723},
                {0.6906502538144703, 0.5529427913432051, 0.4661078162910297},
                {0.6120992573419118, 0.13846786313667225, -0.7785635170233985},
                {-0.6529898983497947, 0.2973682162632619, 0.696546004661236},
                {0.8838990268956842, -0.3557580935792483, -0.30357649629989664},
                {-0.017598789893381296, -0.3812771970649983, 0.9242932335533713},
                {0.6266577852958597, 0.00439502838009063, 0.7792821721646315},
                {-0.4988548127188201, -0.8626471261229718, -0.08356920257516247},
                {-0.17898143512498, -0.252604675059844, 0.9508714550445361},
                {-0.5427075892825453, -0.5030767388258798, -0.6725936866990692},
                {-0.4257387636519463, 0.6171887991146839, -0.6616830747204333},
                {-0.43881682328433247, -0.8421906016741503, 0.31329664226477066},
                {0.6875280177942061, -0.6889379764894024, 0.22949877842519648},
                {-0.010899048699456479, 0.4607053358779587, -0.8874862276289273},
                {0.23217286415626728, 0.6292115743608337, -0.7417469621372482},
                {0.2057475615652692, -0.9451241634400093, 0.2537878180523418},
                {0.7973467562060687, -0.48830125146931924, -0.3546830108437064},
                {-0.8270904423248808, -0.3916496710254459, -0.40315249645821755},
                {-0.4516111403837328, -0.44813753211266727, 0.7715051070428973},
                {0.29433454161327816, 0.9504108972643671, -0.10043059282132165},
                {-0.06759406753980113, 0.16205737210261162, 0.9844635342056203},
                {0.7333647807204668, 0.6286636170107954, 0.25876273890520923},
                {0.9685828598319405, -0.2485904283709124, 0.007074076769848867},
                {-0.25433514721035677, -0.8858127317794803, 0.3881358487834649},
                {0.4110845295860007, 0.5956616622258671, -0.6900700643335961},
                {-0.619090411817337, 0.1451680395344877, 0.7717857878281729},
                {0.649511508220758, -0.6609223729573014, -0.37592075975301764},
                {0.2545386690886485, 0.9343755071236015, -0.2493039863822718},
                {-0.4943741144250881, -0.04714636448965366, 0.8679697317889712},
                {0.7871991054452083, -0.6129556598808209, 0.06784487752462429},
                {0.855092692542928, 0.1803186749724388, 0.4861086942401563},
                {-0.9716984528423624, -0.17891338896543818, -0.15424693187438457},
                {0.6773883555951437, -0.7308388695511149, 0.0837828291319633},
                {0.17320641768183367, -0.7582121151304376, 0.6285808821013046},
                {0.32153148759113853, -0.0964058528139189, 0.9419784573070928},
                {0.2839097714007817, 0.03038580098586635, 0.9583694197967729},
                {0.2647892352954041, 0.11264922678312622, -0.9577039274101569},
                {-0.517092623999111, -0.8267083945381285, 0.22173959638707752},
                {-0.9411108118754085, -0.09179591320530422, -0.3253981408828483},
                {0.9975818371937417, 0.007616233318467676, 0.06908307384010516},
                {0.7218622209806047, 0.5584958860615831, 0.4086530058291947},
                {-0.43001592920010456, 0.776158634115277, 0.461155153199526}};
//end

float random_grad(int hash, float x, float y, float z) 
{
    return grads[hash].x * x + grads[hash].y * y + grads[hash].z * z; 
}

float perlin_3d(float x, float y, float z) 
{
    int nx = (int)(floorf(x)) & 255; //lattice corner (limit to 255 for hashing)
    int ny = (int)(floorf(y)) & 255;
    int nz = (int)(floorf(z)) & 255;

    float sx = x - (floorf(x)); //relative position in lattice
    float sy = y - (floorf(y));
    float sz = z - (floorf(z));

    float u = fade(sx); //fade curve
    float v = fade(sy);
    float w = fade(sz);

    int a = permutation[nx] + ny; //hash values as defined by Ken Perlin
    int aa = permutation[a] + nz;
    int ab = permutation[a + 1] + nz;
    int b = permutation[nx + 1] + ny;
    int ba = permutation[b] + nz;
    int bb = permutation[b + 1] + nz;

    float noise = lerp(w, lerp(v, lerp(u, random_grad(permutation[aa], sx, sy, sz),
                                   random_grad(permutation[ba], sx - 1, sy, sz)), 
                           lerp(u, random_grad(permutation[ab], sx, sy - 1, sz),
                                   random_grad(permutation[bb], sx - 1, sy - 1, sz))), 
                   lerp(v, lerp(u, random_grad(permutation[(aa + 1) & 255], sx, sy, sz - 1),
                                   random_grad(permutation[(ba + 1) & 255], sx - 1, sy, sz - 1)), 
                           lerp(u, random_grad(permutation[(ab + 1) & 255], sx, sy - 1, sz - 1),
                                   random_grad(permutation[(bb + 1) & 255], sx - 1, sy - 1, sz - 1))));

    return noise;
}

//FBM

float fbm_max(unsigned int octaves, float g, float amp, float noise_max) 
{
    float result = 0;
    for (int i = 0; i < octaves; i++) 
    {
        result += amp * noise_max;
        amp *= g;
    }
    return result;
}

float fbm_min(unsigned int octaves, float g, float amp, float noise_min) 
{
    float result = 0;
    for (int i = 0; i < octaves; i++) 
    {
        result += amp * noise_min;
        amp *= g;
    }
    return result;
}

float fbm(unsigned int octaves, float l, float g, float amp, float freq, float x, float y, float z, float (*noise)(float, float, float)) 
{

    float result = 0;
    for (int i = 0; i < octaves; i++) 
    {
        result += amp * noise(freq * x, freq * y, freq * z);
        freq *= l;
        amp *= g;
    }
    return result;
}