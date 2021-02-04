/*

  Dictionary Example 02
  Copyright (c) Anatoli Arkhipenko, 2020
  All Rights Reserved

  This example test Dictionary functionality between various CRC options
  and memory options: RAM or PSRAM (if present).
  
  Compile and run on ESP32 boards only

*/


//#define _DICT_CRC 16
#define _DICT_CRC 32
//#define _DICT_CRC 64

#define _DICT_PACK_STRUCTURES
//#define _DICT_COMPRESS_SHOCO
//#define _DICT_COMPRESS_SMAZ
#define _DICT_USE_PSRAM

//#define _DICT_KEYLEN 64
//#define _DICT_VALLEN 256

#ifdef _DICT_USE_PSRAM
#define KEYS  10000
#define TESTS 10000
#else
#define KEYS  2000
#define TESTS 10000
#endif

#define _DEBUG_
//#define _TEST_

#ifdef _DEBUG_
#define _PP(a) Serial.print(a);
#define _PL(a) Serial.println(a);
#else
#define _PP(a)
#define _PL(a)
#endif

#include <Dictionary.h>

// A list of 1000 random words
// to construct key-value pairs
const char* WORDS[] = {
  "subdued",
  "unequal",
  "toes",
  "invincible",
  "dry",
  "roof",
  "delightful",
  "gather",
  "ripe",
  "approve",
  "occur",
  "dust",
  "slope",
  "hang",
  "utopian",
  "scintillating",
  "magenta",
  "abandoned",
  "icy",
  "hum",
  "truculent",
  "squirrel",
  "exist",
  "piquant",
  "enchanting",
  "hapless",
  "slip",
  "caption",
  "faded",
  "near",
  "stereotyped",
  "uptight",
  "makeshift",
  "scale",
  "aromatic",
  "taboo",
  "flowery",
  "greedy",
  "military",
  "x-ray",
  "rake",
  "rinse",
  "striped",
  "new",
  "permit",
  "belief",
  "buzz",
  "acid",
  "goofy",
  "flawless",
  "impress",
  "coal",
  "settle",
  "tired",
  "smelly",
  "vagabond",
  "afterthought",
  "belligerent",
  "arch",
  "regular",
  "balance",
  "needle",
  "voice",
  "mourn",
  "blind",
  "self",
  "toothbrush",
  "borrow",
  "death",
  "sweet",
  "match",
  "thread",
  "whimsical",
  "cracker",
  "knot",
  "bitter",
  "hurry",
  "surround",
  "bow",
  "deer",
  "steadfast",
  "female",
  "pizzas",
  "gusty",
  "saw",
  "uneven",
  "lyrical",
  "governor",
  "knowing",
  "club",
  "watch",
  "five",
  "pet",
  "towering",
  "bite-sized",
  "outgoing",
  "torpid",
  "bee",
  "miniature",
  "dangerous",
  "pour",
  "redundant",
  "lush",
  "hollow",
  "metal",
  "return",
  "gaze",
  "aunt",
  "foot",
  "damp",
  "key",
  "foolish",
  "earsplitting",
  "hilarious",
  "swing",
  "psychedelic",
  "whine",
  "defeated",
  "rot",
  "expand",
  "basket",
  "intend",
  "apparel",
  "filthy",
  "swift",
  "legal",
  "peaceful",
  "replace",
  "sea",
  "long",
  "spare",
  "hesitant",
  "wrap",
  "sense",
  "trot",
  "flock",
  "complain",
  "deep",
  "obedient",
  "march",
  "vast",
  "spark",
  "staking",
  "disagreeable",
  "milk",
  "ear",
  "rude",
  "heady",
  "trust",
  "cheerful",
  "cry",
  "nostalgic",
  "damage",
  "rose",
  "flimsy",
  "label",
  "fix",
  "brother",
  "hulking",
  "sheep",
  "scream",
  "ignore",
  "brawny",
  "bit",
  "magnificent",
  "describe",
  "youthful",
  "top",
  "press",
  "hands",
  "basin",
  "shocking",
  "rub",
  "van",
  "stage",
  "dinner",
  "ban",
  "royal",
  "reminiscent",
  "wiry",
  "phobic",
  "actually",
  "fetch",
  "flower",
  "plantation",
  "painstaking",
  "laughable",
  "listen",
  "hall",
  "decisive",
  "company",
  "hideous",
  "waiting",
  "drag",
  "disarm",
  "high-pitched",
  "crown",
  "cent",
  "note",
  "rob",
  "enter",
  "attract",
  "can",
  "spoon",
  "ladybug",
  "start",
  "aware",
  "rapid",
  "unable",
  "eatable",
  "aboard",
  "thick",
  "adaptable",
  "comparison",
  "carpenter",
  "consider",
  "kindhearted",
  "physical",
  "elegant",
  "alive",
  "remove",
  "relation",
  "beam",
  "paste",
  "add",
  "page",
  "oceanic",
  "girl",
  "book",
  "cold",
  "thundering",
  "necessary",
  "historical",
  "shape",
  "busy",
  "vegetable",
  "spotty",
  "stupid",
  "premium",
  "tranquil",
  "scare",
  "highfalutin",
  "dispensable",
  "lame",
  "yard",
  "market",
  "taste",
  "late",
  "liquid",
  "meeting",
  "cap",
  "spiffy",
  "closed",
  "trace",
  "riddle",
  "disgusting",
  "discover",
  "odd",
  "stocking",
  "didactic",
  "escape",
  "peel",
  "wacky",
  "list",
  "dogs",
  "whirl",
  "enthusiastic",
  "pop",
  "answer",
  "suspect",
  "loving",
  "purpose",
  "search",
  "punishment",
  "action",
  "useless",
  "incompetent",
  "breakable",
  "automatic",
  "cat",
  "strong",
  "cool",
  "explode",
  "bead",
  "toothsome",
  "pancake",
  "greasy",
  "fool",
  "cruel",
  "better",
  "week",
  "car",
  "elite",
  "push",
  "stew",
  "awake",
  "popcorn",
  "meat",
  "quince",
  "cakes",
  "observe",
  "wretched",
  "nonchalant",
  "earth",
  "pets",
  "point",
  "irate",
  "repeat",
  "reject",
  "bucket",
  "absorbing",
  "fasten",
  "writing",
  "gratis",
  "zebra",
  "jeans",
  "crate",
  "hand",
  "boast",
  "eminent",
  "crook",
  "robust",
  "tail",
  "spot",
  "bikes",
  "structure",
  "pies",
  "wistful",
  "faulty",
  "blushing",
  "decide",
  "calculator",
  "bounce",
  "history",
  "arm",
  "scene",
  "repair",
  "pear",
  "nest",
  "educated",
  "cobweb",
  "auspicious",
  "pleasant",
  "grass",
  "happen",
  "tidy",
  "story",
  "creator",
  "mere",
  "fog",
  "enchanted",
  "idiotic",
  "end",
  "zealous",
  "cure",
  "grape",
  "temporary",
  "compare",
  "dinosaurs",
  "dream",
  "monkey",
  "shallow",
  "past",
  "command",
  "underwear",
  "smiling",
  "invention",
  "button",
  "lock",
  "lick",
  "incandescent",
  "sticky",
  "dead",
  "suck",
  "flood",
  "dare",
  "unlock",
  "month",
  "barbarous",
  "giant",
  "pass",
  "rejoice",
  "handsome",
  "weather",
  "spray",
  "lip",
  "committee",
  "lowly",
  "part",
  "agonizing",
  "small",
  "please",
  "friction",
  "letter",
  "daffy",
  "horn",
  "placid",
  "same",
  "recondite",
  "level",
  "bloody",
  "ill",
  "low",
  "minister",
  "grouchy",
  "wakeful",
  "ablaze",
  "large",
  "thankful",
  "shake",
  "defective",
  "chalk",
  "air",
  "afford",
  "imperfect",
  "dear",
  "zip",
  "entertaining",
  "design",
  "smart",
  "attractive",
  "lacking",
  "cattle",
  "sand",
  "grin",
  "dashing",
  "grandmother",
  "available",
  "plot",
  "grease",
  "tank",
  "longing",
  "kind",
  "snails",
  "delirious",
  "melodic",
  "tremendous",
  "snow",
  "stimulating",
  "volleyball",
  "wool",
  "wren",
  "shop",
  "plain",
  "calendar",
  "existence",
  "axiomatic",
  "cheer",
  "mailbox",
  "burn",
  "advice",
  "tiger",
  "help",
  "reason",
  "erect",
  "hard",
  "stick",
  "disapprove",
  "cub",
  "snakes",
  "weary",
  "sneaky",
  "debonair",
  "tongue",
  "exultant",
  "festive",
  "wreck",
  "deliver",
  "uttermost",
  "ossified",
  "extra-small",
  "selfish",
  "crabby",
  "children",
  "naive",
  "teeny-tiny",
  "needless",
  "stuff",
  "supply",
  "windy",
  "earn",
  "magic",
  "transport",
  "deeply",
  "print",
  "queen",
  "clover",
  "brave",
  "labored",
  "haircut",
  "momentous",
  "frightening",
  "bright",
  "garrulous",
  "trite",
  "truthful",
  "annoying",
  "teaching",
  "nose",
  "size",
  "rhetorical",
  "abundant",
  "language",
  "beginner",
  "inexpensive",
  "industry",
  "dime",
  "industrious",
  "overconfident",
  "lopsided",
  "grade",
  "oval",
  "berry",
  "straw",
  "support",
  "boil",
  "tramp",
  "sneeze",
  "angry",
  "tick",
  "succinct",
  "distribution",
  "unwieldy",
  "serious",
  "adjoining",
  "verse",
  "vein",
  "mask",
  "far-flung",
  "regret",
  "sock",
  "throne",
  "quiet",
  "innate",
  "stove",
  "magical",
  "guard",
  "song",
  "warn",
  "judge",
  "awesome",
  "ritzy",
  "rule",
  "steer",
  "absurd",
  "race",
  "itch",
  "fact",
  "good",
  "frame",
  "nation",
  "zoo",
  "appliance",
  "sort",
  "back",
  "hobbies",
  "substantial",
  "leather",
  "error",
  "believe",
  "toy",
  "cows",
  "selective",
  "power",
  "scattered",
  "multiply",
  "zippy",
  "acoustics",
  "education",
  "madly",
  "sassy",
  "shaky",
  "vague",
  "greet",
  "bells",
  "rings",
  "cooing",
  "unequaled",
  "reduce",
  "mushy",
  "unhealthy",
  "feeble",
  "honey",
  "addicted",
  "sun",
  "fuel",
  "test",
  "racial",
  "clam",
  "ancient",
  "boiling",
  "grumpy",
  "shirt",
  "jewel",
  "space",
  "woozy",
  "pickle",
  "arrogant",
  "baby",
  "tent",
  "calculating",
  "willing",
  "deafening",
  "lunchroom",
  "heavenly",
  "lighten",
  "account",
  "stay",
  "kindly",
  "pedal",
  "fresh",
  "mark",
  "oatmeal",
  "quarrelsome",
  "quill",
  "panicky",
  "waste",
  "wax",
  "easy",
  "amusing",
  "live",
  "bury",
  "unfasten",
  "groan",
  "white",
  "spell",
  "bumpy",
  "womanly",
  "carry",
  "murder",
  "kettle",
  "faint",
  "lumber",
  "wealthy",
  "spotless",
  "hot",
  "marry",
  "helpful",
  "nappy",
  "receipt",
  "accessible",
  "title",
  "possessive",
  "crack",
  "chicken",
  "freezing",
  "awful",
  "neck",
  "accurate",
  "wander",
  "ten",
  "tree",
  "things",
  "unit",
  "perfect",
  "useful",
  "decorous",
  "squeamish",
  "unique",
  "steep",
  "learned",
  "art",
  "sink",
  "eye",
  "direction",
  "program",
  "egg",
  "wriggle",
  "spiteful",
  "park",
  "examine",
  "well-to-do",
  "toothpaste",
  "bag",
  "sick",
  "anger",
  "electric",
  "precede",
  "attempt",
  "planes",
  "choke",
  "difficult",
  "drown",
  "steel",
  "vase",
  "hope",
  "kneel",
  "whip",
  "billowy",
  "testy",
  "cloistered",
  "broad",
  "zipper",
  "wealth",
  "jellyfish",
  "drain",
  "unkempt",
  "mice",
  "cheat",
  "daily",
  "geese",
  "cherries",
  "sticks",
  "driving",
  "argument",
  "squash",
  "addition",
  "simple",
  "effect",
  "potato",
  "quicksand",
  "gullible",
  "meal",
  "educate",
  "incredible",
  "front",
  "snake",
  "various",
  "inform",
  "stain",
  "copy",
  "rampant",
  "peck",
  "unbecoming",
  "strange",
  "stem",
  "nine",
  "drunk",
  "scared",
  "need",
  "rice",
  "beds",
  "melted",
  "extra-large",
  "wrong",
  "horse",
  "blue-eyed",
  "son",
  "glue",
  "jumpy",
  "powerful",
  "juvenile",
  "abject",
  "prepare",
  "forgetful",
  "plumless",
  "dam",
  "preserve",
  "airport",
  "loaf",
  "marked",
  "encouraging",
  "outrageous",
  "spiky",
  "numerous",
  "distance",
  "fill",
  "grain",
  "enormous",
  "minor",
  "adorable",
  "noisy",
  "sturdy",
  "expansion",
  "hellish",
  "splendid",
  "resolute",
  "snotty",
  "wonder",
  "exercise",
  "well-made",
  "victorious",
  "entertain",
  "internal",
  "guttural",
  "present",
  "pollution",
  "capable",
  "cup",
  "apologise",
  "circle",
  "demonic",
  "window",
  "door",
  "paddle",
  "damaging",
  "cowardly",
  "creature",
  "canvas",
  "summer",
  "ruddy",
  "buckeroo",
  "delicious",
  "spicy",
  "observation",
  "aloof",
  "hissing",
  "medical",
  "best",
  "floor",
  "crazy",
  "pray",
  "red",
  "milky",
  "wrench",
  "terrify",
  "sharp",
  "naughty",
  "wasteful",
  "pinch",
  "actor",
  "treatment",
  "comfortable",
  "sour",
  "daughter",
  "run",
  "groovy",
  "stomach",
  "common",
  "paper",
  "organic",
  "glib",
  "admit",
  "sweater",
  "ring",
  "ludicrous",
  "brass",
  "edge",
  "whole",
  "immense",
  "important",
  "happy",
  "strengthen",
  "excuse",
  "wilderness",
  "diligent",
  "zany",
  "truck",
  "ashamed",
  "blue",
  "jazzy",
  "unsightly",
  "zinc",
  "wobble",
  "show",
  "efficient",
  "chop",
  "town",
  "nifty",
  "sisters",
  "soak",
  "elderly",
  "confess",
  "team",
  "yellow",
  "poison",
  "secretary",
  "wide",
  "ocean",
  "natural",
  "try",
  "giddy",
  "aftermath",
  "retire",
  "impartial",
  "cynical",
  "soggy",
  "heat",
  "amused",
  "purring",
  "tempt",
  "abrupt",
  "party",
  "yawn",
  "futuristic",
  "ambiguous",
  "honorable",
  "attend",
  "relieved",
  "peep",
  "celery",
  "suspend",
  "fish",
  "boot",
  "maniacal",
  "disagree",
  "suggestion",
  "apathetic",
  "pause",
  "temper",
  "mitten",
  "huge",
  "gaping",
  "whistle",
  "distinct",
  "knee",
  "delay",
  "expect",
  "carriage",
  "ultra",
  "develop",
  "thumb",
  "afraid",
  "way",
  "old",
  "elastic",
  "tiresome",
  "mute",
  "massive",
  "trick",
  "cross",
  "matter",
  "satisfying",
  "moon",
  "knife",
  "puffy",
  "expensive",
  "offer",
  "trees",
  "hallowed",
  "boundless",
  "tremble",
  "foamy",
  "protect",
  "tearful",
  "advise",
  "dark",
  "tumble",
  "second",
  "gaudy",
  "flow",
  "jobless",
  "tug",
  "workable",
  "stormy",
  "produce",
  "wicked",
  "fang",
  "careless",
  "suggest",
  "excite",
  "vengeful",
  "knit",
  "witty",
  "free",
  "insurance",
  "leg",
  "tray",
  "impolite",
  "feeling",
  "numberless",
  "dirty",
  "wait",
  "gainful",
  "knowledge",
  "glistening",
  "unarmed",
  "talk",
  "dull",
  "humorous",
  "bouncy",
  "sprout",
  "pigs",
  "cream",
  "government",
  "juggle",
  "harbor",
  "hospital",
  "jelly",
  "health",
  "agreeable",
  "idea",
  "parallel",
  "periodic",
  "arrest",
  "call",
  "memory",
  "flowers",
  "special",
  "great",
  "vacation",
  "symptomatic",
  "locket",
  "society",
  "authority",
  "long-term",
  "river",
  "communicate",
  "avoid",
  "condition",
  "divide",
  "slippery",
  "pin",
  "card",
  "scorch",
  "country",
  "clean",
  "maddening",
  "scent",
  "bomb"
};
// ======================================================================

// Print Dictionary contents and statisticts
// or just Statistics
void printD(Dictionary& a, bool justStats = false) {
  _PL();
  if (!justStats) {
    _PL("Dictionary contents:");
    for (int i = 0; i < a.count(); i++) {
      _PP('\t'); _PP(a(i)); _PP(" : "); _PL(a[i]);
    }
  }
  _PP("Current size = "); _PL(a.size());
  _PP("Current count = "); _PL(a.count());
  _PL();
}

// Print current memory parameters
void printH() {
  _PL();
  _PL("Memory stats:");
  _PP("\tTotal  heap: "); _PL(ESP.getHeapSize());
  _PP("\tFree   heap: "); _PL(ESP.getFreeHeap());
  if (psramFound()) {
    _PP("\tTotal PSRAM: "); _PL(ESP.getPsramSize());
    _PP("\tFree  PSRAM: "); _PL(ESP.getFreePsram());
  }
}


void setup() {

  Serial.begin(115200);
  delay(500);
  {
    _PL("Dictionary test and benchmark for ESP32");
    _PL();
  }

  _PL("\n *** FUNCTIONAL TEST ***\n");

  Dictionary* d_ptr = new Dictionary(10);
  Dictionary& d = *d_ptr;


  d("ssid", "devices");
  d("pwd", "********");
  d("url", "http://ota.home.net");
  d("port", "80");
  d("plumless", "plumless value");
  d("buckeroo", "buckeroo value");

  _PL("TEST #1: Inserts");
  printD(d);

  _PL("\nTEST #2: Lookups");
  _PL(d["ssid"]);
  _PL(d["pwd"]);
  _PL(d["url"]);
  _PL(d["port"]);
  _PL(d["plumless"]);
  _PL(d["buckeroo"]);
  _PL(d["does not exist"]);

  _PL("\nTEST #3: Replaces");
  d("plumless", "plumless");
  d("buckeroo", "buckeroo new value");
  printD(d);

  _PL("\nTEST #4: Deletes");
  d.remove("pwd");
  d.remove("buckeroo");
  printD(d);

  delete d_ptr;





  // === Stress test
  _PL("\n *** STRESS TEST ***\n");
  _PL();
  _PP("Inserting  "); _PP(KEYS); _PL(" keys");
  _PP("Performing "); _PP(TESTS); _PL(" lookups"); _PL();
  
  _PP("Heap (start): "); printH();

  char kb[32];
  char kv[256];

  Dictionary* dict = new Dictionary(10);
  Dictionary& t = *dict;

  _PP("Heap (dict created): "); printH();

  _PL("\nPre-populating the dictionary");
  for (int i = 0; i < KEYS; i++) {
    sprintf(kb, "%s-%s", WORDS[random(0, 1000)], WORDS[random(0, 1000)]);
    sprintf(kv, "%s %s %s %s", WORDS[random(0, 1000)], WORDS[random(0, 1000)], WORDS[random(0, 1000)], WORDS[random(0, 1000)]);
    if (t(String(kb), String(kv))) {
      Serial.printf("Error inserting %d - %s : %s\n", i, kb, kv);
      printH();
      for (;;);
    }
    if (!t(kb) || t[kb] != kv) {
      Serial.printf("K-V mismatch %d - %s : %s\n", i, kb, kv);
      _PP(t(kb)); _PP(" : "); _PL(t[kb]);
      printH();
      delay(5000);
    }
  }
  _PP("Heap (After pre-population): ");  printH();

  printD(t, true);

  _PL("Example key-value pair:");
  _PP(t(0)); _PP(" : "); _PL(t[0]); _PL();

  _PL("Measuring lookup base loop");
  unsigned long t1 = micros();
  for (int i = 0; i < TESTS; i++) {
    sprintf(kb, "%s-%s", WORDS[random(0, 1000)], WORDS[random(0, 1000)]);
  }
  unsigned long t2 = micros();

  _PP("Heap (After base lookup): ");  printH();

  _PL("Measuring lookup actual loop");
  unsigned long t3 = micros();
  for (int i = 0; i < TESTS; i++) {
    sprintf(kb, "%s%s", WORDS[random(0, 1000)], WORDS[random(0, 1000)]);
    t[kb];
  }
  unsigned long t4 = micros();

  _PP("Heap (After lookup actual loop): ");  printH();

  _PL("Measuring delete base loop");
  int l = t.count() - 1;
  unsigned long t5 = micros();
  for (int i = 0; i < KEYS; i++) {
    String k = t(l--);
  }
  unsigned long t6 = micros();

  _PL("Measuring delete actual loop");
  l = t.count() - 1;
  unsigned long t7 = micros();
  for (int i = 0; i < KEYS; i++) {
    String k = t(l--);
    t.remove(k);
  }
  unsigned long t8 = micros();
  
  _PP("Heap (After delete loops): ");  printH();


  long dt2 = ((t8 - t7) - (t6 - t5));
  float rate2 = (float)dt2 / (float)KEYS;

  long dt = ((t4 - t3) - (t2 - t1));
  float rate = (float)dt / (float)TESTS;


  Serial.print("Lookup duration: ");
  Serial.print(dt);
  Serial.print(" micros, rate=");
  Serial.print(rate);
  Serial.println(" micros/lookup");

  Serial.print("Delete duration: ");
  Serial.print(dt2);
  Serial.print(" micros, rate=");
  Serial.print(rate2);
  Serial.println(" micros/delete");

  delete dict;
  _PP("Heap (After dict destroy): ");  printH();
  _PL("*** END OF THE TEST ***");
}

void loop() {
}
