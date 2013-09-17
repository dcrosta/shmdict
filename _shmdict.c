#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include <Python.h>

typedef enum {
    DETACHED = 0,
    ATTACHED = 1
} shmdict_state;

typedef struct {
    int id;
    void *base;
    shmdict_state state;
} shmdict_segment;


static PyObject*
shmdict_attach(PyObject* self, PyObject* args)
{
    char *shmdict_py_filename;
    int segment_id;
    unsigned long segment_size;
    key_t segment_key;
    shmdict_segment *segment;

    if (!PyArg_ParseTuple(args, "sik", &shmdict_py_filename, &segment_id, &segment_size))
        return NULL;

    segment = malloc(sizeof(shmdict_segment));
    if (NULL == segment)
        return PyErr_NoMemory();

    segment->id = 0;
    segment->base = NULL;
    segment->state = DETACHED;

    segment_key = ftok(shmdict_py_filename, segment_id);
    segment->id = shmget(segment_key, segment_size, 0644 | IPC_CREAT);
    if (-1 == segment->id) {
        free(segment);
        return PyErr_SetFromErrno(PyExc_OSError);
    }

    segment->base = shmat(segment->id, 0, 0);
    if (-1 == (int)segment->base) {
        free(segment);
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    segment->state = ATTACHED;

    return PyInt_FromLong((unsigned long)(void*)segment);
}

static PyObject*
shmdict_detach(PyObject* self, PyObject* args)
{
    unsigned long segment_addr;
    void *base;
    shmdict_segment *segment;

    if (!PyArg_ParseTuple(args, "k", &segment_addr))
        return NULL;

    segment = (void*)segment_addr;
    if (ATTACHED != segment->state) {
        PyErr_SetString(PyExc_ValueError, "segment is not attached");
        return NULL;
    }

    base = segment->base;
    segment->base = NULL;
    segment->state = DETACHED;

    if (-1 == shmdt(base))
        return PyErr_SetFromErrno(PyExc_OSError);

    Py_RETURN_NONE;
}

static PyObject*
shmdict_delete(PyObject* self, PyObject* args)
{
    unsigned long segment_addr;
    int segment_id;
    shmdict_segment *segment;

    if (!PyArg_ParseTuple(args, "k", &segment_addr))
        return NULL;

    segment = (void*)segment_addr;
    if (DETACHED != segment->state) {
        PyErr_SetString(PyExc_ValueError, "segment is attached");
        return NULL;
    }

    segment_id = segment->id;
    free(segment);

    if (-1 == shmctl(segment->id, IPC_RMID, NULL))
        return PyErr_SetFromErrno(PyExc_OSError);

    Py_RETURN_NONE;
}


static PyMethodDef SHMDictMethods[] = {
    {"attach", shmdict_attach, METH_VARARGS, "attach a shared memory segment"},
    {"detach", shmdict_detach, METH_VARARGS, "detach a shared memory segment"},
    {"delete", shmdict_delete, METH_VARARGS, "delete a shared memory segment"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
init_shmdict(void)
{
    Py_InitModule("_shmdict", SHMDictMethods);
}
