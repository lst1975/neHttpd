// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.
#ifndef RYU_D2S_FULL_TABLE_H
#define RYU_D2S_FULL_TABLE_H

// These tables are generated by PrintDoubleLookupTable.
#define DOUBLE_POW5_INV_BITCOUNT 125
#define DOUBLE_POW5_BITCOUNT 125

#define DOUBLE_POW5_INV_TABLE_SIZE 342
#define DOUBLE_POW5_TABLE_SIZE 326

static const uint64_t DOUBLE_POW5_INV_SPLIT[DOUBLE_POW5_INV_TABLE_SIZE][2] = {
  {                    1ull, 2305843009213693952ull }, { 11068046444225730970ull, 1844674407370955161ull },
  {  5165088340638674453ull, 1475739525896764129ull }, {  7821419487252849886ull, 1180591620717411303ull },
  {  8824922364862649494ull, 1888946593147858085ull }, {  7059937891890119595ull, 1511157274518286468ull },
  { 13026647942995916322ull, 1208925819614629174ull }, {  9774590264567735146ull, 1934281311383406679ull },
  { 11509021026396098440ull, 1547425049106725343ull }, { 16585914450600699399ull, 1237940039285380274ull },
  { 15469416676735388068ull, 1980704062856608439ull }, { 16064882156130220778ull, 1584563250285286751ull },
  {  9162556910162266299ull, 1267650600228229401ull }, {  7281393426775805432ull, 2028240960365167042ull },
  { 16893161185646375315ull, 1622592768292133633ull }, {  2446482504291369283ull, 1298074214633706907ull },
  {  7603720821608101175ull, 2076918743413931051ull }, {  2393627842544570617ull, 1661534994731144841ull },
  { 16672297533003297786ull, 1329227995784915872ull }, { 11918280793837635165ull, 2126764793255865396ull },
  {  5845275820328197809ull, 1701411834604692317ull }, { 15744267100488289217ull, 1361129467683753853ull },
  {  3054734472329800808ull, 2177807148294006166ull }, { 17201182836831481939ull, 1742245718635204932ull },
  {  6382248639981364905ull, 1393796574908163946ull }, {  2832900194486363201ull, 2230074519853062314ull },
  {  5955668970331000884ull, 1784059615882449851ull }, {  1075186361522890384ull, 1427247692705959881ull },
  { 12788344622662355584ull, 2283596308329535809ull }, { 13920024512871794791ull, 1826877046663628647ull },
  {  3757321980813615186ull, 1461501637330902918ull }, { 10384555214134712795ull, 1169201309864722334ull },
  {  5547241898389809503ull, 1870722095783555735ull }, {  4437793518711847602ull, 1496577676626844588ull },
  { 10928932444453298728ull, 1197262141301475670ull }, { 17486291911125277965ull, 1915619426082361072ull },
  {  6610335899416401726ull, 1532495540865888858ull }, { 12666966349016942027ull, 1225996432692711086ull },
  { 12888448528943286597ull, 1961594292308337738ull }, { 17689456452638449924ull, 1569275433846670190ull },
  { 14151565162110759939ull, 1255420347077336152ull }, {  7885109000409574610ull, 2008672555323737844ull },
  {  9997436015069570011ull, 1606938044258990275ull }, {  7997948812055656009ull, 1285550435407192220ull },
  { 12796718099289049614ull, 2056880696651507552ull }, {  2858676849947419045ull, 1645504557321206042ull },
  { 13354987924183666206ull, 1316403645856964833ull }, { 17678631863951955605ull, 2106245833371143733ull },
  {  3074859046935833515ull, 1684996666696914987ull }, { 13527933681774397782ull, 1347997333357531989ull },
  { 10576647446613305481ull, 2156795733372051183ull }, { 15840015586774465031ull, 1725436586697640946ull },
  {  8982663654677661702ull, 1380349269358112757ull }, { 18061610662226169046ull, 2208558830972980411ull },
  { 10759939715039024913ull, 1766847064778384329ull }, { 12297300586773130254ull, 1413477651822707463ull },
  { 15986332124095098083ull, 2261564242916331941ull }, {  9099716884534168143ull, 1809251394333065553ull },
  { 14658471137111155161ull, 1447401115466452442ull }, {  4348079280205103483ull, 1157920892373161954ull },
  { 14335624477811986218ull, 1852673427797059126ull }, {  7779150767507678651ull, 1482138742237647301ull },
  {  2533971799264232598ull, 1185710993790117841ull }, { 15122401323048503126ull, 1897137590064188545ull },
  { 12097921058438802501ull, 1517710072051350836ull }, {  5988988032009131678ull, 1214168057641080669ull },
  { 16961078480698431330ull, 1942668892225729070ull }, { 13568862784558745064ull, 1554135113780583256ull },
  {  7165741412905085728ull, 1243308091024466605ull }, { 11465186260648137165ull, 1989292945639146568ull },
  { 16550846638002330379ull, 1591434356511317254ull }, { 16930026125143774626ull, 1273147485209053803ull },
  {  4951948911778577463ull, 2037035976334486086ull }, {   272210314680951647ull, 1629628781067588869ull },
  {  3907117066486671641ull, 1303703024854071095ull }, {  6251387306378674625ull, 2085924839766513752ull },
  { 16069156289328670670ull, 1668739871813211001ull }, {  9165976216721026213ull, 1334991897450568801ull },
  {  7286864317269821294ull, 2135987035920910082ull }, { 16897537898041588005ull, 1708789628736728065ull },
  { 13518030318433270404ull, 1367031702989382452ull }, {  6871453250525591353ull, 2187250724783011924ull },
  {  9186511415162383406ull, 1749800579826409539ull }, { 11038557946871817048ull, 1399840463861127631ull },
  { 10282995085511086630ull, 2239744742177804210ull }, {  8226396068408869304ull, 1791795793742243368ull },
  { 13959814484210916090ull, 1433436634993794694ull }, { 11267656730511734774ull, 2293498615990071511ull },
  {  5324776569667477496ull, 1834798892792057209ull }, {  7949170070475892320ull, 1467839114233645767ull },
  { 17427382500606444826ull, 1174271291386916613ull }, {  5747719112518849781ull, 1878834066219066582ull },
  { 15666221734240810795ull, 1503067252975253265ull }, { 12532977387392648636ull, 1202453802380202612ull },
  {  5295368560860596524ull, 1923926083808324180ull }, {  4236294848688477220ull, 1539140867046659344ull },
  {  7078384693692692099ull, 1231312693637327475ull }, { 11325415509908307358ull, 1970100309819723960ull },
  {  9060332407926645887ull, 1576080247855779168ull }, { 14626963555825137356ull, 1260864198284623334ull },
  { 12335095245094488799ull, 2017382717255397335ull }, {  9868076196075591040ull, 1613906173804317868ull },
  { 15273158586344293478ull, 1291124939043454294ull }, { 13369007293925138595ull, 2065799902469526871ull },
  {  7005857020398200553ull, 1652639921975621497ull }, { 16672732060544291412ull, 1322111937580497197ull },
  { 11918976037903224966ull, 2115379100128795516ull }, {  5845832015580669650ull, 1692303280103036413ull },
  { 12055363241948356366ull, 1353842624082429130ull }, {   841837113407818570ull, 2166148198531886609ull },
  {  4362818505468165179ull, 1732918558825509287ull }, { 14558301248600263113ull, 1386334847060407429ull },
  { 12225235553534690011ull, 2218135755296651887ull }, {  2401490813343931363ull, 1774508604237321510ull },
  {  1921192650675145090ull, 1419606883389857208ull }, { 17831303500047873437ull, 2271371013423771532ull },
  {  6886345170554478103ull, 1817096810739017226ull }, {  1819727321701672159ull, 1453677448591213781ull },
  { 16213177116328979020ull, 1162941958872971024ull }, { 14873036941900635463ull, 1860707134196753639ull },
  { 15587778368262418694ull, 1488565707357402911ull }, {  8780873879868024632ull, 1190852565885922329ull },
  {  2981351763563108441ull, 1905364105417475727ull }, { 13453127855076217722ull, 1524291284333980581ull },
  {  7073153469319063855ull, 1219433027467184465ull }, { 11317045550910502167ull, 1951092843947495144ull },
  { 12742985255470312057ull, 1560874275157996115ull }, { 10194388204376249646ull, 1248699420126396892ull },
  {  1553625868034358140ull, 1997919072202235028ull }, {  8621598323911307159ull, 1598335257761788022ull },
  { 17965325103354776697ull, 1278668206209430417ull }, { 13987124906400001422ull, 2045869129935088668ull },
  {   121653480894270168ull, 1636695303948070935ull }, {    97322784715416134ull, 1309356243158456748ull },
  { 14913111714512307107ull, 2094969989053530796ull }, {  8241140556867935363ull, 1675975991242824637ull },
  { 17660958889720079260ull, 1340780792994259709ull }, { 17189487779326395846ull, 2145249268790815535ull },
  { 13751590223461116677ull, 1716199415032652428ull }, { 18379969808252713988ull, 1372959532026121942ull },
  { 14650556434236701088ull, 2196735251241795108ull }, {   652398703163629901ull, 1757388200993436087ull },
  { 11589965406756634890ull, 1405910560794748869ull }, {  7475898206584884855ull, 2249456897271598191ull },
  {  2291369750525997561ull, 1799565517817278553ull }, {  9211793429904618695ull, 1439652414253822842ull },
  { 18428218302589300235ull, 2303443862806116547ull }, {  7363877012587619542ull, 1842755090244893238ull },
  { 13269799239553916280ull, 1474204072195914590ull }, { 10615839391643133024ull, 1179363257756731672ull },
  {  2227947767661371545ull, 1886981212410770676ull }, { 16539753473096738529ull, 1509584969928616540ull },
  { 13231802778477390823ull, 1207667975942893232ull }, {  6413489186596184024ull, 1932268761508629172ull },
  { 16198837793502678189ull, 1545815009206903337ull }, {  5580372605318321905ull, 1236652007365522670ull },
  {  8928596168509315048ull, 1978643211784836272ull }, { 18210923379033183008ull, 1582914569427869017ull },
  {  7190041073742725760ull, 1266331655542295214ull }, {   436019273762630246ull, 2026130648867672343ull },
  {  7727513048493924843ull, 1620904519094137874ull }, {  9871359253537050198ull, 1296723615275310299ull },
  {  4726128361433549347ull, 2074757784440496479ull }, {  7470251503888749801ull, 1659806227552397183ull },
  { 13354898832594820487ull, 1327844982041917746ull }, { 13989140502667892133ull, 2124551971267068394ull },
  { 14880661216876224029ull, 1699641577013654715ull }, { 11904528973500979224ull, 1359713261610923772ull },
  {  4289851098633925465ull, 2175541218577478036ull }, { 18189276137874781665ull, 1740432974861982428ull },
  {  3483374466074094362ull, 1392346379889585943ull }, {  1884050330976640656ull, 2227754207823337509ull },
  {  5196589079523222848ull, 1782203366258670007ull }, { 15225317707844309248ull, 1425762693006936005ull },
  {  5913764258841343181ull, 2281220308811097609ull }, {  8420360221814984868ull, 1824976247048878087ull },
  { 17804334621677718864ull, 1459980997639102469ull }, { 17932816512084085415ull, 1167984798111281975ull },
  { 10245762345624985047ull, 1868775676978051161ull }, {  4507261061758077715ull, 1495020541582440929ull },
  {  7295157664148372495ull, 1196016433265952743ull }, {  7982903447895485668ull, 1913626293225524389ull },
  { 10075671573058298858ull, 1530901034580419511ull }, {  4371188443704728763ull, 1224720827664335609ull },
  { 14372599139411386667ull, 1959553324262936974ull }, { 15187428126271019657ull, 1567642659410349579ull },
  { 15839291315758726049ull, 1254114127528279663ull }, {  3206773216762499739ull, 2006582604045247462ull },
  { 13633465017635730761ull, 1605266083236197969ull }, { 14596120828850494932ull, 1284212866588958375ull },
  {  4907049252451240275ull, 2054740586542333401ull }, {   236290587219081897ull, 1643792469233866721ull },
  { 14946427728742906810ull, 1315033975387093376ull }, { 16535586736504830250ull, 2104054360619349402ull },
  {  5849771759720043554ull, 1683243488495479522ull }, { 15747863852001765813ull, 1346594790796383617ull },
  { 10439186904235184007ull, 2154551665274213788ull }, { 15730047152871967852ull, 1723641332219371030ull },
  { 12584037722297574282ull, 1378913065775496824ull }, {  9066413911450387881ull, 2206260905240794919ull },
  { 10942479943902220628ull, 1765008724192635935ull }, {  8753983955121776503ull, 1412006979354108748ull },
  { 10317025513452932081ull, 2259211166966573997ull }, {   874922781278525018ull, 1807368933573259198ull },
  {  8078635854506640661ull, 1445895146858607358ull }, { 13841606313089133175ull, 1156716117486885886ull },
  { 14767872471458792434ull, 1850745787979017418ull }, {   746251532941302978ull, 1480596630383213935ull },
  {   597001226353042382ull, 1184477304306571148ull }, { 15712597221132509104ull, 1895163686890513836ull },
  {  8880728962164096960ull, 1516130949512411069ull }, { 10793931984473187891ull, 1212904759609928855ull },
  { 17270291175157100626ull, 1940647615375886168ull }, {  2748186495899949531ull, 1552518092300708935ull },
  {  2198549196719959625ull, 1242014473840567148ull }, { 18275073973719576693ull, 1987223158144907436ull },
  { 10930710364233751031ull, 1589778526515925949ull }, { 12433917106128911148ull, 1271822821212740759ull },
  {  8826220925580526867ull, 2034916513940385215ull }, {  7060976740464421494ull, 1627933211152308172ull },
  { 16716827836597268165ull, 1302346568921846537ull }, { 11989529279587987770ull, 2083754510274954460ull },
  {  9591623423670390216ull, 1667003608219963568ull }, { 15051996368420132820ull, 1333602886575970854ull },
  { 13015147745246481542ull, 2133764618521553367ull }, {  3033420566713364587ull, 1707011694817242694ull },
  {  6116085268112601993ull, 1365609355853794155ull }, {  9785736428980163188ull, 2184974969366070648ull },
  { 15207286772667951197ull, 1747979975492856518ull }, {  1097782973908629988ull, 1398383980394285215ull },
  {  1756452758253807981ull, 2237414368630856344ull }, {  5094511021344956708ull, 1789931494904685075ull },
  {  4075608817075965366ull, 1431945195923748060ull }, {  6520974107321544586ull, 2291112313477996896ull },
  {  1527430471115325346ull, 1832889850782397517ull }, { 12289990821117991246ull, 1466311880625918013ull },
  { 17210690286378213644ull, 1173049504500734410ull }, {  9090360384495590213ull, 1876879207201175057ull },
  { 18340334751822203140ull, 1501503365760940045ull }, { 14672267801457762512ull, 1201202692608752036ull },
  { 16096930852848599373ull, 1921924308174003258ull }, {  1809498238053148529ull, 1537539446539202607ull },
  { 12515645034668249793ull, 1230031557231362085ull }, {  1578287981759648052ull, 1968050491570179337ull },
  { 12330676829633449412ull, 1574440393256143469ull }, { 13553890278448669853ull, 1259552314604914775ull },
  {  3239480371808320148ull, 2015283703367863641ull }, { 17348979556414297411ull, 1612226962694290912ull },
  {  6500486015647617283ull, 1289781570155432730ull }, { 10400777625036187652ull, 2063650512248692368ull },
  { 15699319729512770768ull, 1650920409798953894ull }, { 16248804598352126938ull, 1320736327839163115ull },
  {  7551343283653851484ull, 2113178124542660985ull }, {  6041074626923081187ull, 1690542499634128788ull },
  { 12211557331022285596ull, 1352433999707303030ull }, {  1091747655926105338ull, 2163894399531684849ull },
  {  4562746939482794594ull, 1731115519625347879ull }, {  7339546366328145998ull, 1384892415700278303ull },
  {  8053925371383123274ull, 2215827865120445285ull }, {  6443140297106498619ull, 1772662292096356228ull },
  { 12533209867169019542ull, 1418129833677084982ull }, {  5295740528502789974ull, 2269007733883335972ull },
  { 15304638867027962949ull, 1815206187106668777ull }, {  4865013464138549713ull, 1452164949685335022ull },
  { 14960057215536570740ull, 1161731959748268017ull }, {  9178696285890871890ull, 1858771135597228828ull },
  { 14721654658196518159ull, 1487016908477783062ull }, {  4398626097073393881ull, 1189613526782226450ull },
  {  7037801755317430209ull, 1903381642851562320ull }, {  5630241404253944167ull, 1522705314281249856ull },
  {   814844308661245011ull, 1218164251424999885ull }, {  1303750893857992017ull, 1949062802279999816ull },
  { 15800395974054034906ull, 1559250241823999852ull }, {  5261619149759407279ull, 1247400193459199882ull },
  { 12107939454356961969ull, 1995840309534719811ull }, {  5997002748743659252ull, 1596672247627775849ull },
  {  8486951013736837725ull, 1277337798102220679ull }, {  2511075177753209390ull, 2043740476963553087ull },
  { 13076906586428298482ull, 1634992381570842469ull }, { 14150874083884549109ull, 1307993905256673975ull },
  {  4194654460505726958ull, 2092790248410678361ull }, { 18113118827372222859ull, 1674232198728542688ull },
  {  3422448617672047318ull, 1339385758982834151ull }, { 16543964232501006678ull, 2143017214372534641ull },
  {  9545822571258895019ull, 1714413771498027713ull }, { 15015355686490936662ull, 1371531017198422170ull },
  {  5577825024675947042ull, 2194449627517475473ull }, { 11840957649224578280ull, 1755559702013980378ull },
  { 16851463748863483271ull, 1404447761611184302ull }, { 12204946739213931940ull, 2247116418577894884ull },
  { 13453306206113055875ull, 1797693134862315907ull }, {  3383947335406624054ull, 1438154507889852726ull },
  { 16482362180876329456ull, 2301047212623764361ull }, {  9496540929959153242ull, 1840837770099011489ull },
  { 11286581558709232917ull, 1472670216079209191ull }, {  5339916432225476010ull, 1178136172863367353ull },
  {  4854517476818851293ull, 1885017876581387765ull }, {  3883613981455081034ull, 1508014301265110212ull },
  { 14174937629389795797ull, 1206411441012088169ull }, { 11611853762797942306ull, 1930258305619341071ull },
  {  5600134195496443521ull, 1544206644495472857ull }, { 15548153800622885787ull, 1235365315596378285ull },
  {  6430302007287065643ull, 1976584504954205257ull }, { 16212288050055383484ull, 1581267603963364205ull },
  { 12969830440044306787ull, 1265014083170691364ull }, {  9683682259845159889ull, 2024022533073106183ull },
  { 15125643437359948558ull, 1619218026458484946ull }, {  8411165935146048523ull, 1295374421166787957ull },
  { 17147214310975587960ull, 2072599073866860731ull }, { 10028422634038560045ull, 1658079259093488585ull },
  {  8022738107230848036ull, 1326463407274790868ull }, {  9147032156827446534ull, 2122341451639665389ull },
  { 11006974540203867551ull, 1697873161311732311ull }, {  5116230817421183718ull, 1358298529049385849ull },
  { 15564666937357714594ull, 2173277646479017358ull }, {  1383687105660440706ull, 1738622117183213887ull },
  { 12174996128754083534ull, 1390897693746571109ull }, {  8411947361780802685ull, 2225436309994513775ull },
  {  6729557889424642148ull, 1780349047995611020ull }, {  5383646311539713719ull, 1424279238396488816ull },
  {  1235136468979721303ull, 2278846781434382106ull }, { 15745504434151418335ull, 1823077425147505684ull },
  { 16285752362063044992ull, 1458461940118004547ull }, {  5649904260166615347ull, 1166769552094403638ull },
  {  5350498001524674232ull, 1866831283351045821ull }, {   591049586477829062ull, 1493465026680836657ull },
  { 11540886113407994219ull, 1194772021344669325ull }, {    18673707743239135ull, 1911635234151470921ull },
  { 14772334225162232601ull, 1529308187321176736ull }, {  8128518565387875758ull, 1223446549856941389ull },
  {  1937583260394870242ull, 1957514479771106223ull }, {  8928764237799716840ull, 1566011583816884978ull },
  { 14521709019723594119ull, 1252809267053507982ull }, {  8477339172590109297ull, 2004494827285612772ull },
  { 17849917782297818407ull, 1603595861828490217ull }, {  6901236596354434079ull, 1282876689462792174ull },
  { 18420676183650915173ull, 2052602703140467478ull }, {  3668494502695001169ull, 1642082162512373983ull },
  { 10313493231639821582ull, 1313665730009899186ull }, {  9122891541139893884ull, 2101865168015838698ull },
  { 14677010862395735754ull, 1681492134412670958ull }, {   673562245690857633ull, 1345193707530136767ull }
};

static const uint64_t DOUBLE_POW5_SPLIT[DOUBLE_POW5_TABLE_SIZE][2] = {
  {                    0ull, 1152921504606846976ull }, {                    0ull, 1441151880758558720ull },
  {                    0ull, 1801439850948198400ull }, {                    0ull, 2251799813685248000ull },
  {                    0ull, 1407374883553280000ull }, {                    0ull, 1759218604441600000ull },
  {                    0ull, 2199023255552000000ull }, {                    0ull, 1374389534720000000ull },
  {                    0ull, 1717986918400000000ull }, {                    0ull, 2147483648000000000ull },
  {                    0ull, 1342177280000000000ull }, {                    0ull, 1677721600000000000ull },
  {                    0ull, 2097152000000000000ull }, {                    0ull, 1310720000000000000ull },
  {                    0ull, 1638400000000000000ull }, {                    0ull, 2048000000000000000ull },
  {                    0ull, 1280000000000000000ull }, {                    0ull, 1600000000000000000ull },
  {                    0ull, 2000000000000000000ull }, {                    0ull, 1250000000000000000ull },
  {                    0ull, 1562500000000000000ull }, {                    0ull, 1953125000000000000ull },
  {                    0ull, 1220703125000000000ull }, {                    0ull, 1525878906250000000ull },
  {                    0ull, 1907348632812500000ull }, {                    0ull, 1192092895507812500ull },
  {                    0ull, 1490116119384765625ull }, {  4611686018427387904ull, 1862645149230957031ull },
  {  9799832789158199296ull, 1164153218269348144ull }, { 12249790986447749120ull, 1455191522836685180ull },
  { 15312238733059686400ull, 1818989403545856475ull }, { 14528612397897220096ull, 2273736754432320594ull },
  { 13692068767113150464ull, 1421085471520200371ull }, { 12503399940464050176ull, 1776356839400250464ull },
  { 15629249925580062720ull, 2220446049250313080ull }, {  9768281203487539200ull, 1387778780781445675ull },
  {  7598665485932036096ull, 1734723475976807094ull }, {   274959820560269312ull, 2168404344971008868ull },
  {  9395221924704944128ull, 1355252715606880542ull }, {  2520655369026404352ull, 1694065894508600678ull },
  { 12374191248137781248ull, 2117582368135750847ull }, { 14651398557727195136ull, 1323488980084844279ull },
  { 13702562178731606016ull, 1654361225106055349ull }, {  3293144668132343808ull, 2067951531382569187ull },
  { 18199116482078572544ull, 1292469707114105741ull }, {  8913837547316051968ull, 1615587133892632177ull },
  { 15753982952572452864ull, 2019483917365790221ull }, { 12152082354571476992ull, 1262177448353618888ull },
  { 15190102943214346240ull, 1577721810442023610ull }, {  9764256642163156992ull, 1972152263052529513ull },
  { 17631875447420442880ull, 1232595164407830945ull }, {  8204786253993389888ull, 1540743955509788682ull },
  {  1032610780636961552ull, 1925929944387235853ull }, {  2951224747111794922ull, 1203706215242022408ull },
  {  3689030933889743652ull, 1504632769052528010ull }, { 13834660704216955373ull, 1880790961315660012ull },
  { 17870034976990372916ull, 1175494350822287507ull }, { 17725857702810578241ull, 1469367938527859384ull },
  {  3710578054803671186ull, 1836709923159824231ull }, {    26536550077201078ull, 2295887403949780289ull },
  { 11545800389866720434ull, 1434929627468612680ull }, { 14432250487333400542ull, 1793662034335765850ull },
  {  8816941072311974870ull, 2242077542919707313ull }, { 17039803216263454053ull, 1401298464324817070ull },
  { 12076381983474541759ull, 1751623080406021338ull }, {  5872105442488401391ull, 2189528850507526673ull },
  { 15199280947623720629ull, 1368455531567204170ull }, {  9775729147674874978ull, 1710569414459005213ull },
  { 16831347453020981627ull, 2138211768073756516ull }, {  1296220121283337709ull, 1336382355046097823ull },
  { 15455333206886335848ull, 1670477943807622278ull }, { 10095794471753144002ull, 2088097429759527848ull },
  {  6309871544845715001ull, 1305060893599704905ull }, { 12499025449484531656ull, 1631326116999631131ull },
  { 11012095793428276666ull, 2039157646249538914ull }, { 11494245889320060820ull, 1274473528905961821ull },
  {   532749306367912313ull, 1593091911132452277ull }, {  5277622651387278295ull, 1991364888915565346ull },
  {  7910200175544436838ull, 1244603055572228341ull }, { 14499436237857933952ull, 1555753819465285426ull },
  {  8900923260467641632ull, 1944692274331606783ull }, { 12480606065433357876ull, 1215432671457254239ull },
  { 10989071563364309441ull, 1519290839321567799ull }, {  9124653435777998898ull, 1899113549151959749ull },
  {  8008751406574943263ull, 1186945968219974843ull }, {  5399253239791291175ull, 1483682460274968554ull },
  { 15972438586593889776ull, 1854603075343710692ull }, {   759402079766405302ull, 1159126922089819183ull },
  { 14784310654990170340ull, 1448908652612273978ull }, {  9257016281882937117ull, 1811135815765342473ull },
  { 16182956370781059300ull, 2263919769706678091ull }, {  7808504722524468110ull, 1414949856066673807ull },
  {  5148944884728197234ull, 1768687320083342259ull }, {  1824495087482858639ull, 2210859150104177824ull },
  {  1140309429676786649ull, 1381786968815111140ull }, {  1425386787095983311ull, 1727233711018888925ull },
  {  6393419502297367043ull, 2159042138773611156ull }, { 13219259225790630210ull, 1349401336733506972ull },
  { 16524074032238287762ull, 1686751670916883715ull }, { 16043406521870471799ull, 2108439588646104644ull },
  {   803757039314269066ull, 1317774742903815403ull }, { 14839754354425000045ull, 1647218428629769253ull },
  {  4714634887749086344ull, 2059023035787211567ull }, {  9864175832484260821ull, 1286889397367007229ull },
  { 16941905809032713930ull, 1608611746708759036ull }, {  2730638187581340797ull, 2010764683385948796ull },
  { 10930020904093113806ull, 1256727927116217997ull }, { 18274212148543780162ull, 1570909908895272496ull },
  {  4396021111970173586ull, 1963637386119090621ull }, {  5053356204195052443ull, 1227273366324431638ull },
  { 15540067292098591362ull, 1534091707905539547ull }, { 14813398096695851299ull, 1917614634881924434ull },
  { 13870059828862294966ull, 1198509146801202771ull }, { 12725888767650480803ull, 1498136433501503464ull },
  { 15907360959563101004ull, 1872670541876879330ull }, { 14553786618154326031ull, 1170419088673049581ull },
  {  4357175217410743827ull, 1463023860841311977ull }, { 10058155040190817688ull, 1828779826051639971ull },
  {  7961007781811134206ull, 2285974782564549964ull }, { 14199001900486734687ull, 1428734239102843727ull },
  { 13137066357181030455ull, 1785917798878554659ull }, { 11809646928048900164ull, 2232397248598193324ull },
  { 16604401366885338411ull, 1395248280373870827ull }, { 16143815690179285109ull, 1744060350467338534ull },
  { 10956397575869330579ull, 2180075438084173168ull }, {  6847748484918331612ull, 1362547148802608230ull },
  { 17783057643002690323ull, 1703183936003260287ull }, { 17617136035325974999ull, 2128979920004075359ull },
  { 17928239049719816230ull, 1330612450002547099ull }, { 17798612793722382384ull, 1663265562503183874ull },
  { 13024893955298202172ull, 2079081953128979843ull }, {  5834715712847682405ull, 1299426220705612402ull },
  { 16516766677914378815ull, 1624282775882015502ull }, { 11422586310538197711ull, 2030353469852519378ull },
  { 11750802462513761473ull, 1268970918657824611ull }, { 10076817059714813937ull, 1586213648322280764ull },
  { 12596021324643517422ull, 1982767060402850955ull }, {  5566670318688504437ull, 1239229412751781847ull },
  {  2346651879933242642ull, 1549036765939727309ull }, {  7545000868343941206ull, 1936295957424659136ull },
  {  4715625542714963254ull, 1210184973390411960ull }, {  5894531928393704067ull, 1512731216738014950ull },
  { 16591536947346905892ull, 1890914020922518687ull }, { 17287239619732898039ull, 1181821263076574179ull },
  { 16997363506238734644ull, 1477276578845717724ull }, {  2799960309088866689ull, 1846595723557147156ull },
  { 10973347230035317489ull, 1154122327223216972ull }, { 13716684037544146861ull, 1442652909029021215ull },
  { 12534169028502795672ull, 1803316136286276519ull }, { 11056025267201106687ull, 2254145170357845649ull },
  { 18439230838069161439ull, 1408840731473653530ull }, { 13825666510731675991ull, 1761050914342066913ull },
  {  3447025083132431277ull, 2201313642927583642ull }, {  6766076695385157452ull, 1375821026829739776ull },
  {  8457595869231446815ull, 1719776283537174720ull }, { 10571994836539308519ull, 2149720354421468400ull },
  {  6607496772837067824ull, 1343575221513417750ull }, { 17482743002901110588ull, 1679469026891772187ull },
  { 17241742735199000331ull, 2099336283614715234ull }, { 15387775227926763111ull, 1312085177259197021ull },
  {  5399660979626290177ull, 1640106471573996277ull }, { 11361262242960250625ull, 2050133089467495346ull },
  { 11712474920277544544ull, 1281333180917184591ull }, { 10028907631919542777ull, 1601666476146480739ull },
  {  7924448521472040567ull, 2002083095183100924ull }, { 14176152362774801162ull, 1251301934489438077ull },
  {  3885132398186337741ull, 1564127418111797597ull }, {  9468101516160310080ull, 1955159272639746996ull },
  { 15140935484454969608ull, 1221974545399841872ull }, {   479425281859160394ull, 1527468181749802341ull },
  {  5210967620751338397ull, 1909335227187252926ull }, { 17091912818251750210ull, 1193334516992033078ull },
  { 12141518985959911954ull, 1491668146240041348ull }, { 15176898732449889943ull, 1864585182800051685ull },
  { 11791404716994875166ull, 1165365739250032303ull }, { 10127569877816206054ull, 1456707174062540379ull },
  {  8047776328842869663ull, 1820883967578175474ull }, {   836348374198811271ull, 2276104959472719343ull },
  {  7440246761515338900ull, 1422565599670449589ull }, { 13911994470321561530ull, 1778206999588061986ull },
  {  8166621051047176104ull, 2222758749485077483ull }, {  2798295147690791113ull, 1389224218428173427ull },
  { 17332926989895652603ull, 1736530273035216783ull }, { 17054472718942177850ull, 2170662841294020979ull },
  {  8353202440125167204ull, 1356664275808763112ull }, { 10441503050156459005ull, 1695830344760953890ull },
  {  3828506775840797949ull, 2119787930951192363ull }, {    86973725686804766ull, 1324867456844495227ull },
  { 13943775212390669669ull, 1656084321055619033ull }, {  3594660960206173375ull, 2070105401319523792ull },
  {  2246663100128858359ull, 1293815875824702370ull }, { 12031700912015848757ull, 1617269844780877962ull },
  {  5816254103165035138ull, 2021587305976097453ull }, {  5941001823691840913ull, 1263492066235060908ull },
  {  7426252279614801142ull, 1579365082793826135ull }, {  4671129331091113523ull, 1974206353492282669ull },
  {  5225298841145639904ull, 1233878970932676668ull }, {  6531623551432049880ull, 1542348713665845835ull },
  {  3552843420862674446ull, 1927935892082307294ull }, { 16055585193321335241ull, 1204959932551442058ull },
  { 10846109454796893243ull, 1506199915689302573ull }, { 18169322836923504458ull, 1882749894611628216ull },
  { 11355826773077190286ull, 1176718684132267635ull }, {  9583097447919099954ull, 1470898355165334544ull },
  { 11978871809898874942ull, 1838622943956668180ull }, { 14973589762373593678ull, 2298278679945835225ull },
  {  2440964573842414192ull, 1436424174966147016ull }, {  3051205717303017741ull, 1795530218707683770ull },
  { 13037379183483547984ull, 2244412773384604712ull }, {  8148361989677217490ull, 1402757983365377945ull },
  { 14797138505523909766ull, 1753447479206722431ull }, { 13884737113477499304ull, 2191809349008403039ull },
  { 15595489723564518921ull, 1369880843130251899ull }, { 14882676136028260747ull, 1712351053912814874ull },
  {  9379973133180550126ull, 2140438817391018593ull }, { 17391698254306313589ull, 1337774260869386620ull },
  {  3292878744173340370ull, 1672217826086733276ull }, {  4116098430216675462ull, 2090272282608416595ull },
  {   266718509671728212ull, 1306420176630260372ull }, {   333398137089660265ull, 1633025220787825465ull },
  {  5028433689789463235ull, 2041281525984781831ull }, { 10060300083759496378ull, 1275800953740488644ull },
  { 12575375104699370472ull, 1594751192175610805ull }, {  1884160825592049379ull, 1993438990219513507ull },
  { 17318501580490888525ull, 1245899368887195941ull }, {  7813068920331446945ull, 1557374211108994927ull },
  {  5154650131986920777ull, 1946717763886243659ull }, {   915813323278131534ull, 1216698602428902287ull },
  { 14979824709379828129ull, 1520873253036127858ull }, {  9501408849870009354ull, 1901091566295159823ull },
  { 12855909558809837702ull, 1188182228934474889ull }, {  2234828893230133415ull, 1485227786168093612ull },
  {  2793536116537666769ull, 1856534732710117015ull }, {  8663489100477123587ull, 1160334207943823134ull },
  {  1605989338741628675ull, 1450417759929778918ull }, { 11230858710281811652ull, 1813022199912223647ull },
  {  9426887369424876662ull, 2266277749890279559ull }, { 12809333633531629769ull, 1416423593681424724ull },
  { 16011667041914537212ull, 1770529492101780905ull }, {  6179525747111007803ull, 2213161865127226132ull },
  { 13085575628799155685ull, 1383226165704516332ull }, { 16356969535998944606ull, 1729032707130645415ull },
  { 15834525901571292854ull, 2161290883913306769ull }, {  2979049660840976177ull, 1350806802445816731ull },
  { 17558870131333383934ull, 1688508503057270913ull }, {  8113529608884566205ull, 2110635628821588642ull },
  {  9682642023980241782ull, 1319147268013492901ull }, { 16714988548402690132ull, 1648934085016866126ull },
  { 11670363648648586857ull, 2061167606271082658ull }, { 11905663298832754689ull, 1288229753919426661ull },
  {  1047021068258779650ull, 1610287192399283327ull }, { 15143834390605638274ull, 2012858990499104158ull },
  {  4853210475701136017ull, 1258036869061940099ull }, {  1454827076199032118ull, 1572546086327425124ull },
  {  1818533845248790147ull, 1965682607909281405ull }, {  3442426662494187794ull, 1228551629943300878ull },
  { 13526405364972510550ull, 1535689537429126097ull }, {  3072948650933474476ull, 1919611921786407622ull },
  { 15755650962115585259ull, 1199757451116504763ull }, { 15082877684217093670ull, 1499696813895630954ull },
  {  9630225068416591280ull, 1874621017369538693ull }, {  8324733676974063502ull, 1171638135855961683ull },
  {  5794231077790191473ull, 1464547669819952104ull }, {  7242788847237739342ull, 1830684587274940130ull },
  { 18276858095901949986ull, 2288355734093675162ull }, { 16034722328366106645ull, 1430222333808546976ull },
  {  1596658836748081690ull, 1787777917260683721ull }, {  6607509564362490017ull, 2234722396575854651ull },
  {  1823850468512862308ull, 1396701497859909157ull }, {  6891499104068465790ull, 1745876872324886446ull },
  { 17837745916940358045ull, 2182346090406108057ull }, {  4231062170446641922ull, 1363966306503817536ull },
  {  5288827713058302403ull, 1704957883129771920ull }, {  6611034641322878003ull, 2131197353912214900ull },
  { 13355268687681574560ull, 1331998346195134312ull }, { 16694085859601968200ull, 1664997932743917890ull },
  { 11644235287647684442ull, 2081247415929897363ull }, {  4971804045566108824ull, 1300779634956185852ull },
  {  6214755056957636030ull, 1625974543695232315ull }, {  3156757802769657134ull, 2032468179619040394ull },
  {  6584659645158423613ull, 1270292612261900246ull }, { 17454196593302805324ull, 1587865765327375307ull },
  { 17206059723201118751ull, 1984832206659219134ull }, {  6142101308573311315ull, 1240520129162011959ull },
  {  3065940617289251240ull, 1550650161452514949ull }, {  8444111790038951954ull, 1938312701815643686ull },
  {   665883850346957067ull, 1211445438634777304ull }, {   832354812933696334ull, 1514306798293471630ull },
  { 10263815553021896226ull, 1892883497866839537ull }, { 17944099766707154901ull, 1183052186166774710ull },
  { 13206752671529167818ull, 1478815232708468388ull }, { 16508440839411459773ull, 1848519040885585485ull },
  { 12623618533845856310ull, 1155324400553490928ull }, { 15779523167307320387ull, 1444155500691863660ull },
  {  1277659885424598868ull, 1805194375864829576ull }, {  1597074856780748586ull, 2256492969831036970ull },
  {  5609857803915355770ull, 1410308106144398106ull }, { 16235694291748970521ull, 1762885132680497632ull },
  {  1847873790976661535ull, 2203606415850622041ull }, { 12684136165428883219ull, 1377254009906638775ull },
  { 11243484188358716120ull, 1721567512383298469ull }, {   219297180166231438ull, 2151959390479123087ull },
  {  7054589765244976505ull, 1344974619049451929ull }, { 13429923224983608535ull, 1681218273811814911ull },
  { 12175718012802122765ull, 2101522842264768639ull }, { 14527352785642408584ull, 1313451776415480399ull },
  { 13547504963625622826ull, 1641814720519350499ull }, { 12322695186104640628ull, 2052268400649188124ull },
  { 16925056528170176201ull, 1282667750405742577ull }, {  7321262604930556539ull, 1603334688007178222ull },
  { 18374950293017971482ull, 2004168360008972777ull }, {  4566814905495150320ull, 1252605225005607986ull },
  { 14931890668723713708ull, 1565756531257009982ull }, {  9441491299049866327ull, 1957195664071262478ull },
  {  1289246043478778550ull, 1223247290044539049ull }, {  6223243572775861092ull, 1529059112555673811ull },
  {  3167368447542438461ull, 1911323890694592264ull }, {  1979605279714024038ull, 1194577431684120165ull },
  {  7086192618069917952ull, 1493221789605150206ull }, { 18081112809442173248ull, 1866527237006437757ull },
  { 13606538515115052232ull, 1166579523129023598ull }, {  7784801107039039482ull, 1458224403911279498ull },
  {   507629346944023544ull, 1822780504889099373ull }, {  5246222702107417334ull, 2278475631111374216ull },
  {  3278889188817135834ull, 1424047269444608885ull }, {  8710297504448807696ull, 1780059086805761106ull }
};

#endif // RYU_D2S_FULL_TABLE_H
