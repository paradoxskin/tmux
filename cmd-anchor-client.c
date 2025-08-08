/* $OpenBSD$ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicholas.marriott@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "tmux.h"

/*
 * Make an anchor from a client to another client
 */

static enum cmd_retval	cmd_anchor_client_exec(struct cmd *, struct cmdq_item *);

const struct cmd_entry cmd_anchor_client_entry = {
	.name = "anchor-client",
	.alias = NULL,

	.args = { "ds:t:", 0, 0, NULL },
	.usage = CMD_SRCDST_CLIENT_USAGE,

	.flags = CMD_CLIENT_TFLAG,
	.exec = cmd_anchor_client_exec
};

static enum cmd_retval
cmd_anchor_client_exec(struct cmd *self, struct cmdq_item *item)
{
	struct args		*args = cmd_get_args(self);
	struct client		*tc = cmdq_get_target_client(item);
	struct client		*sc = cmd_find_client(item, NULL, 1);

	if (args_has(args, 's')) {
		sc = cmd_find_client(item, args_get(args, 's'), 0);
	}

	if ((sc == NULL) || (tc == NULL)) {
		return (CMD_RETURN_ERROR);
	}

	if (args_has(args, 'd')) {
		sc->anchor_client = NULL;
		cmdq_print(item, "clear anchor of %s", sc->name);
		return (CMD_RETURN_NORMAL);
	}

	if (sc == tc) {
		cmdq_print(item, "source and target are same: %s", tc->name);
		return (CMD_RETURN_ERROR);
	}

	if (tc->anchor_client) {
		cmdq_print(item, "target: %s already has an anchor: %s", tc->name, tc->anchor_client->name);
		return (CMD_RETURN_ERROR);
	}

	cmdq_print(item, "anchor %s -> %s", sc->name, tc->name);
	sc->anchor_client = tc;

	return (CMD_RETURN_NORMAL);
}
