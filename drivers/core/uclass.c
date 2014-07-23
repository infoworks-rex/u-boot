/*
 * Copyright (c) 2013 Google, Inc
 *
 * (C) Copyright 2012
 * Pavel Herrmann <morpheus.ibis@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <dm/device.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/uclass.h>
#include <dm/uclass-internal.h>
#include <dm/util.h>

DECLARE_GLOBAL_DATA_PTR;

struct uclass *uclass_find(enum uclass_id key)
{
	struct uclass *uc;

	/*
	 * TODO(sjg@chromium.org): Optimise this, perhaps moving the found
	 * node to the start of the list, or creating a linear array mapping
	 * id to node.
	 */
	list_for_each_entry(uc, &gd->uclass_root, sibling_node) {
		if (uc->uc_drv->id == key)
			return uc;
	}

	return NULL;
}

/**
 * uclass_add() - Create new uclass in list
 * @id: Id number to create
 * @ucp: Returns pointer to uclass, or NULL on error
 * @return 0 on success, -ve on error
 *
 * The new uclass is added to the list. There must be only one uclass for
 * each id.
 */
static int uclass_add(enum uclass_id id, struct uclass **ucp)
{
	struct uclass_driver *uc_drv;
	struct uclass *uc;
	int ret;

	*ucp = NULL;
	uc_drv = lists_uclass_lookup(id);
	if (!uc_drv) {
		dm_warn("Cannot find uclass for id %d: please add the UCLASS_DRIVER() declaration for this UCLASS_... id\n",
			id);
		return -ENOENT;
	}
	if (uc_drv->ops) {
		dm_warn("No ops for uclass id %d\n", id);
		return -EINVAL;
	}
	uc = calloc(1, sizeof(*uc));
	if (!uc)
		return -ENOMEM;
	if (uc_drv->priv_auto_alloc_size) {
		uc->priv = calloc(1, uc_drv->priv_auto_alloc_size);
		if (!uc->priv) {
			ret = -ENOMEM;
			goto fail_mem;
		}
	}
	uc->uc_drv = uc_drv;
	INIT_LIST_HEAD(&uc->sibling_node);
	INIT_LIST_HEAD(&uc->dev_head);
	list_add(&uc->sibling_node, &DM_UCLASS_ROOT_NON_CONST);

	if (uc_drv->init) {
		ret = uc_drv->init(uc);
		if (ret)
			goto fail;
	}

	*ucp = uc;

	return 0;
fail:
	if (uc_drv->priv_auto_alloc_size) {
		free(uc->priv);
		uc->priv = NULL;
	}
	list_del(&uc->sibling_node);
fail_mem:
	free(uc);

	return ret;
}

int uclass_destroy(struct uclass *uc)
{
	struct uclass_driver *uc_drv;
	struct udevice *dev, *tmp;
	int ret;

	list_for_each_entry_safe(dev, tmp, &uc->dev_head, uclass_node) {
		ret = device_remove(dev);
		if (ret)
			return ret;
		ret = device_unbind(dev);
		if (ret)
			return ret;
	}

	uc_drv = uc->uc_drv;
	if (uc_drv->destroy)
		uc_drv->destroy(uc);
	list_del(&uc->sibling_node);
	if (uc_drv->priv_auto_alloc_size)
		free(uc->priv);
	free(uc);

	return 0;
}

int uclass_get(enum uclass_id id, struct uclass **ucp)
{
	struct uclass *uc;

	*ucp = NULL;
	uc = uclass_find(id);
	if (!uc)
		return uclass_add(id, ucp);
	*ucp = uc;

	return 0;
}

int uclass_find_device(enum uclass_id id, int index, struct udevice **devp)
{
	struct uclass *uc;
	struct udevice *dev;
	int ret;

	*devp = NULL;
	ret = uclass_get(id, &uc);
	if (ret)
		return ret;

	list_for_each_entry(dev, &uc->dev_head, uclass_node) {
		if (!index--) {
			*devp = dev;
			return 0;
		}
	}

	return -ENODEV;
}

int uclass_find_device_by_seq(enum uclass_id id, int seq_or_req_seq,
			      bool find_req_seq, struct udevice **devp)
{
	struct uclass *uc;
	struct udevice *dev;
	int ret;

	*devp = NULL;
	debug("%s: %d %d\n", __func__, find_req_seq, seq_or_req_seq);
	if (seq_or_req_seq == -1)
		return -ENODEV;
	ret = uclass_get(id, &uc);
	if (ret)
		return ret;

	list_for_each_entry(dev, &uc->dev_head, uclass_node) {
		debug("   - %d %d\n", dev->req_seq, dev->seq);
		if ((find_req_seq ? dev->req_seq : dev->seq) ==
				seq_or_req_seq) {
			*devp = dev;
			debug("   - found\n");
			return 0;
		}
	}
	debug("   - not found\n");

	return -ENODEV;
}

/**
 * uclass_get_device_tail() - handle the end of a get_device call
 *
 * This handles returning an error or probing a device as needed.
 *
 * @dev: Device that needs to be probed
 * @ret: Error to return. If non-zero then the device is not probed
 * @devp: Returns the value of 'dev' if there is no error
 * @return ret, if non-zero, else the result of the device_probe() call
 */
static int uclass_get_device_tail(struct udevice *dev, int ret,
				  struct udevice **devp)
{
	if (ret)
		return ret;

	ret = device_probe(dev);
	if (ret)
		return ret;

	*devp = dev;

	return 0;
}

int uclass_get_device(enum uclass_id id, int index, struct udevice **devp)
{
	struct udevice *dev;
	int ret;

	*devp = NULL;
	ret = uclass_find_device(id, index, &dev);
	return uclass_get_device_tail(dev, ret, devp);
}

int uclass_get_device_by_seq(enum uclass_id id, int seq, struct udevice **devp)
{
	struct udevice *dev;
	int ret;

	*devp = NULL;
	ret = uclass_find_device_by_seq(id, seq, false, &dev);
	if (ret == -ENODEV) {
		/*
		 * We didn't find it in probed devices. See if there is one
		 * that will request this seq if probed.
		 */
		ret = uclass_find_device_by_seq(id, seq, true, &dev);
	}
	return uclass_get_device_tail(dev, ret, devp);
}

int uclass_first_device(enum uclass_id id, struct udevice **devp)
{
	struct uclass *uc;
	struct udevice *dev;
	int ret;

	*devp = NULL;
	ret = uclass_get(id, &uc);
	if (ret)
		return ret;
	if (list_empty(&uc->dev_head))
		return 0;

	dev = list_first_entry(&uc->dev_head, struct udevice, uclass_node);
	ret = device_probe(dev);
	if (ret)
		return ret;
	*devp = dev;

	return 0;
}

int uclass_next_device(struct udevice **devp)
{
	struct udevice *dev = *devp;
	int ret;

	*devp = NULL;
	if (list_is_last(&dev->uclass_node, &dev->uclass->dev_head))
		return 0;

	dev = list_entry(dev->uclass_node.next, struct udevice,
			 uclass_node);
	ret = device_probe(dev);
	if (ret)
		return ret;
	*devp = dev;

	return 0;
}

int uclass_bind_device(struct udevice *dev)
{
	struct uclass *uc;
	int ret;

	uc = dev->uclass;

	list_add_tail(&dev->uclass_node, &uc->dev_head);

	if (uc->uc_drv->post_bind) {
		ret = uc->uc_drv->post_bind(dev);
		if (ret) {
			list_del(&dev->uclass_node);
			return ret;
		}
	}

	return 0;
}

int uclass_unbind_device(struct udevice *dev)
{
	struct uclass *uc;
	int ret;

	uc = dev->uclass;
	if (uc->uc_drv->pre_unbind) {
		ret = uc->uc_drv->pre_unbind(dev);
		if (ret)
			return ret;
	}

	list_del(&dev->uclass_node);
	return 0;
}

int uclass_resolve_seq(struct udevice *dev)
{
	struct udevice *dup;
	int seq;
	int ret;

	assert(dev->seq == -1);
	ret = uclass_find_device_by_seq(dev->uclass->uc_drv->id, dev->req_seq,
					false, &dup);
	if (!ret) {
		dm_warn("Device '%s': seq %d is in use by '%s'\n",
			dev->name, dev->req_seq, dup->name);
	} else if (ret == -ENODEV) {
		/* Our requested sequence number is available */
		if (dev->req_seq != -1)
			return dev->req_seq;
	} else {
		return ret;
	}

	for (seq = 0; seq < DM_MAX_SEQ; seq++) {
		ret = uclass_find_device_by_seq(dev->uclass->uc_drv->id, seq,
						false, &dup);
		if (ret == -ENODEV)
			break;
		if (ret)
			return ret;
	}
	return seq;
}

int uclass_post_probe_device(struct udevice *dev)
{
	struct uclass_driver *uc_drv = dev->uclass->uc_drv;

	if (uc_drv->post_probe)
		return uc_drv->post_probe(dev);

	return 0;
}

int uclass_pre_remove_device(struct udevice *dev)
{
	struct uclass_driver *uc_drv;
	struct uclass *uc;
	int ret;

	uc = dev->uclass;
	uc_drv = uc->uc_drv;
	if (uc->uc_drv->pre_remove) {
		ret = uc->uc_drv->pre_remove(dev);
		if (ret)
			return ret;
	}
	if (uc_drv->per_device_auto_alloc_size) {
		free(dev->uclass_priv);
		dev->uclass_priv = NULL;
	}
	dev->seq = -1;

	return 0;
}
