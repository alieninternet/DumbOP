
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Note - Wrapper into the note interface
 */
void Commands::Note(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendNotice("The note interface is not operational.");
}
